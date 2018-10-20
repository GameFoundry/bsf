//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanCommandBuffer.h"
#include "Managers/BsVulkanCommandBufferManager.h"
#include "BsVulkanUtility.h"
#include "BsVulkanDevice.h"
#include "BsVulkanGpuParams.h"
#include "BsVulkanQueue.h"
#include "BsVulkanTexture.h"
#include "BsVulkanIndexBuffer.h"
#include "BsVulkanVertexBuffer.h"
#include "BsVulkanHardwareBuffer.h"
#include "BsVulkanFramebuffer.h"
#include "Managers/BsVulkanVertexInputManager.h"
#include "BsVulkanEventQuery.h"
#include "Managers/BsVulkanQueryManager.h"
#include "BsVulkanSwapChain.h"
#include "BsVulkanTimerQuery.h"
#include "BsVulkanOcclusionQuery.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include "Win32/BsWin32RenderWindow.h"
#elif BS_PLATFORM == BS_PLATFORM_LINUX
#include "Linux/BsLinuxRenderWindow.h"
#elif BS_PLATFORM == BS_PLATFORM_OSX
#include "MacOS/BsMacOSRenderWindow.h"
#else
static_assert(false, "Other platforms go here");
#endif

namespace bs { namespace ct
{
	VulkanSemaphore::VulkanSemaphore(VulkanResourceManager* owner)
		:VulkanResource(owner, true)
	{
		VkSemaphoreCreateInfo semaphoreCI;
		semaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCI.pNext = nullptr;
		semaphoreCI.flags = 0;

		VkResult result = vkCreateSemaphore(owner->getDevice().getLogical(), &semaphoreCI, gVulkanAllocator, &mSemaphore);
		assert(result == VK_SUCCESS);
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
		vkDestroySemaphore(mOwner->getDevice().getLogical(), mSemaphore, gVulkanAllocator);
	}

	VulkanCmdBufferPool::VulkanCmdBufferPool(VulkanDevice& device)
		:mDevice(device), mNextId(1)
	{
		for (UINT32 i = 0; i < GQT_COUNT; i++)
		{
			UINT32 familyIdx = device.getQueueFamily((GpuQueueType)i);

			if (familyIdx == (UINT32)-1)
				continue;

			VkCommandPoolCreateInfo poolCI;
			poolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolCI.pNext = nullptr;
			poolCI.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolCI.queueFamilyIndex = familyIdx;

			PoolInfo& poolInfo = mPools[familyIdx];
			poolInfo.queueFamily = familyIdx;
			memset(poolInfo.buffers, 0, sizeof(poolInfo.buffers));

			vkCreateCommandPool(device.getLogical(), &poolCI, gVulkanAllocator, &poolInfo.pool);
		}
	}

	VulkanCmdBufferPool::~VulkanCmdBufferPool()
	{
		// Note: Shutdown should be the only place command buffers are destroyed at, as the system relies on the fact that
		// they won't be destroyed during normal operation.

		for(auto& entry : mPools)
		{
			PoolInfo& poolInfo = entry.second;
			for (UINT32 i = 0; i < BS_MAX_VULKAN_CB_PER_QUEUE_FAMILY; i++)
			{
				VulkanCmdBuffer* buffer = poolInfo.buffers[i];
				if (buffer == nullptr)
					break;

				bs_delete(buffer);
			}

			vkDestroyCommandPool(mDevice.getLogical(), poolInfo.pool, gVulkanAllocator);
		}
	}

	VulkanCmdBuffer* VulkanCmdBufferPool::getBuffer(UINT32 queueFamily, bool secondary)
	{
		auto iterFind = mPools.find(queueFamily);
		if (iterFind == mPools.end())
			return nullptr;

		VulkanCmdBuffer** buffers = iterFind->second.buffers;

		UINT32 i = 0;
		for(; i < BS_MAX_VULKAN_CB_PER_QUEUE_FAMILY; i++)
		{
			if (buffers[i] == nullptr)
				break;

			if(buffers[i]->mState == VulkanCmdBuffer::State::Ready)
			{
				buffers[i]->begin();
				return buffers[i];
			}
		}

		assert(i < BS_MAX_VULKAN_CB_PER_QUEUE_FAMILY &&
			"Too many command buffers allocated. Increment BS_MAX_VULKAN_CB_PER_QUEUE_FAMILY to a higher value. ");

		buffers[i] = createBuffer(queueFamily, secondary);
		buffers[i]->begin();

		return buffers[i];
	}

	VulkanCmdBuffer* VulkanCmdBufferPool::createBuffer(UINT32 queueFamily, bool secondary)
	{
		auto iterFind = mPools.find(queueFamily);
		if (iterFind == mPools.end())
			return nullptr;

		const PoolInfo& poolInfo = iterFind->second;

		return bs_new<VulkanCmdBuffer>(mDevice, mNextId++, poolInfo.pool, poolInfo.queueFamily, secondary);
	}

	/** Returns a set of pipeline stages that can are allowed to be used for the specified set of access flags. */
	VkPipelineStageFlags getPipelineStageFlags(VkAccessFlags accessFlags)
	{
		VkPipelineStageFlags flags = 0;

		if ((accessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

		if ((accessFlags & (VK_ACCESS_INDEX_READ_BIT | VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

		if ((accessFlags & (VK_ACCESS_UNIFORM_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT)) != 0)
		{
			flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT | VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}

		if ((accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT) != 0)
			flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		if ((accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		if ((accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		if ((accessFlags & (VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

		if ((accessFlags & (VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT)) != 0)
			flags |= VK_PIPELINE_STAGE_HOST_BIT;

		if (flags == 0)
			flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		return flags;
	}

	template<class T>
	void getPipelineStageFlags(const Vector<T>& barriers, VkPipelineStageFlags& src, VkPipelineStageFlags& dst)
	{
		for(auto& entry : barriers)
		{
			src |= getPipelineStageFlags(entry.srcAccessMask);
			dst |= getPipelineStageFlags(entry.dstAccessMask);
		}

		if (src == 0)
			src = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

		if (dst == 0)
			dst = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
	}

	VulkanCmdBuffer::VulkanCmdBuffer(VulkanDevice& device, UINT32 id, VkCommandPool pool, UINT32 queueFamily, bool secondary)
		: mId(id), mQueueFamily(queueFamily), mState(State::Ready), mDevice(device), mPool(pool)
		, mIntraQueueSemaphore(nullptr), mInterQueueSemaphores(), mNumUsedInterQueueSemaphores(0)
		, mFramebuffer(nullptr), mRenderTargetReadOnlyFlags(0), mRenderTargetLoadMask(RT_NONE), mGlobalQueueIdx(-1)
		, mViewport(0.0f, 0.0f, 1.0f, 1.0f), mScissor(0, 0, 0, 0), mStencilRef(0), mDrawOp(DOT_TRIANGLE_LIST)
		, mNumBoundDescriptorSets(0), mGfxPipelineRequiresBind(true), mCmpPipelineRequiresBind(true)
		, mViewportRequiresBind(true), mStencilRefRequiresBind(true), mScissorRequiresBind(true), mBoundParamsDirty(false)
		, mClearValues(), mClearMask(), mSemaphoresTemp(BS_MAX_UNIQUE_QUEUES), mVertexBuffersTemp()
		, mVertexBufferOffsetsTemp()
	{
		UINT32 maxBoundDescriptorSets = device.getDeviceProperties().limits.maxBoundDescriptorSets;
		mDescriptorSetsTemp = (VkDescriptorSet*)bs_alloc(sizeof(VkDescriptorSet) * maxBoundDescriptorSets);

		VkCommandBufferAllocateInfo cmdBufferAllocInfo;
		cmdBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdBufferAllocInfo.pNext = nullptr;
		cmdBufferAllocInfo.commandPool = pool;
		cmdBufferAllocInfo.level = secondary ? VK_COMMAND_BUFFER_LEVEL_SECONDARY : VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdBufferAllocInfo.commandBufferCount = 1;

		VkResult result = vkAllocateCommandBuffers(mDevice.getLogical(), &cmdBufferAllocInfo, &mCmdBuffer);
		assert(result == VK_SUCCESS);

		VkFenceCreateInfo fenceCI;
		fenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCI.pNext = nullptr;
		fenceCI.flags = 0;

		result = vkCreateFence(mDevice.getLogical(), &fenceCI, gVulkanAllocator, &mFence);
		assert(result == VK_SUCCESS);
	}

	VulkanCmdBuffer::~VulkanCmdBuffer()
	{
		VkDevice device = mDevice.getLogical();

		if(mState == State::Submitted)
		{
			// Wait 1s
			UINT64 waitTime = 1000 * 1000 * 1000;
			VkResult result = vkWaitForFences(device, 1, &mFence, true, waitTime);
			assert(result == VK_SUCCESS || result == VK_TIMEOUT);

			if (result == VK_TIMEOUT)
				LOGWRN("Freeing a command buffer before done executing because fence wait expired!");

			// Resources have been marked as used, make sure to notify them we're done with them
			reset();
		}
		else if(mState != State::Ready) 
		{
			// Notify any resources that they are no longer bound
			for (auto& entry : mResources)
			{
				ResourceUseHandle& useHandle = entry.second;
				assert(!useHandle.used);

				entry.first->notifyUnbound();
			}

			for (auto& entry : mImages)
			{
				UINT32 imageInfoIdx = entry.second;
				ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

				ResourceUseHandle& useHandle = imageInfo.useHandle;
				assert(!useHandle.used);

				entry.first->notifyUnbound();
			}

			for (auto& entry : mBuffers)
			{
				ResourceUseHandle& useHandle = entry.second.useHandle;
				assert(!useHandle.used);

				entry.first->notifyUnbound();
			}
		}

		if (mIntraQueueSemaphore != nullptr)
			mIntraQueueSemaphore->destroy();
		
		for(UINT32 i = 0; i < BS_MAX_VULKAN_CB_DEPENDENCIES; i++)
		{
			if (mInterQueueSemaphores[i] != nullptr)
				mInterQueueSemaphores[i]->destroy();
		}

		vkDestroyFence(device, mFence, gVulkanAllocator);
		vkFreeCommandBuffers(device, mPool, 1, &mCmdBuffer);

		bs_free(mDescriptorSetsTemp);
	}

	UINT32 VulkanCmdBuffer::getDeviceIdx() const
	{
		return mDevice.getIndex();
	}

	void VulkanCmdBuffer::begin()
	{
		assert(mState == State::Ready);

		VkCommandBufferBeginInfo beginInfo;
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.pNext = nullptr;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		VkResult result = vkBeginCommandBuffer(mCmdBuffer, &beginInfo);
		assert(result == VK_SUCCESS);

		mState = State::Recording;
	}

	void VulkanCmdBuffer::end()
	{
		assert(mState == State::Recording);

		// If a clear is queued, execute the render pass with no additional instructions
		if (mClearMask)
			executeClearPass();

		VkResult result = vkEndCommandBuffer(mCmdBuffer);
		assert(result == VK_SUCCESS);

		mState = State::RecordingDone;
	}

	void VulkanCmdBuffer::beginRenderPass()
	{
		assert(mState == State::Recording);

		if (mFramebuffer == nullptr)
		{
			LOGWRN("Attempting to begin a render pass but no render target is bound to the command buffer.");
			return;
		}

		if(mClearMask != CLEAR_NONE)
		{
			// If a previous clear is queued, but it doesn't match the rendered area, need to execute a separate pass
			// just for it
			Rect2I rtArea(0, 0, mFramebuffer->getWidth(), mFramebuffer->getHeight());
			if (mClearArea != rtArea)
				executeClearPass();
		}

		executeLayoutTransitions();

		RenderSurfaceMask readMask = getFBReadMask();

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.framebuffer = mFramebuffer->getFramebuffer(mRenderTargetLoadMask, readMask, mClearMask);
		renderPassBeginInfo.renderPass = mFramebuffer->getRenderPass(mRenderTargetLoadMask, readMask, mClearMask);
		renderPassBeginInfo.renderArea.offset.x = 0;
		renderPassBeginInfo.renderArea.offset.y = 0;
		renderPassBeginInfo.renderArea.extent.width = mFramebuffer->getWidth();
		renderPassBeginInfo.renderArea.extent.height = mFramebuffer->getHeight();
		renderPassBeginInfo.clearValueCount = mFramebuffer->getNumClearEntries(mClearMask);
		renderPassBeginInfo.pClearValues = mClearValues.data();

		vkCmdBeginRenderPass(mCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

		mClearMask = CLEAR_NONE;
		mState = State::RecordingRenderPass;
	}

	void VulkanCmdBuffer::endRenderPass()
	{
		assert(mState == State::RecordingRenderPass);

		vkCmdEndRenderPass(mCmdBuffer);

		// Execute any queued events
		for(auto& entry : mQueuedEvents)
			vkCmdSetEvent(mCmdBuffer, entry->getHandle(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		mQueuedEvents.clear();

		// Update any layout transitions that were performed by subpass dependencies, reset flags that signal image usage
		// and reset read-only state.
		for(auto& entry : mPassTouchedSubresourceInfos)
		{
			ImageSubresourceInfo& subresourceInfo = mSubresourceInfoStorage[entry];
			subresourceInfo.isShaderInput = false;
			subresourceInfo.isReadOnly = true;
			subresourceInfo.needsBarrier = false;
		}

		for (auto& entry : mBuffers)
			entry.second.needsBarrier = false;

		mPassTouchedSubresourceInfos.clear();

		updateFinalLayouts();

		mState = State::Recording;

		// In case the same GPU params from last pass get used, this makes sure the states we reset above, get re-applied
		mBoundParamsDirty = true;
	}

	void VulkanCmdBuffer::allocateSemaphores(VkSemaphore* semaphores)
	{
		if (mIntraQueueSemaphore != nullptr)
			mIntraQueueSemaphore->destroy();

		mIntraQueueSemaphore = mDevice.getResourceManager().create<VulkanSemaphore>();
		semaphores[0] = mIntraQueueSemaphore->getHandle();

		for (UINT32 i = 0; i < BS_MAX_VULKAN_CB_DEPENDENCIES; i++)
		{
			if (mInterQueueSemaphores[i] != nullptr)
				mInterQueueSemaphores[i]->destroy();

			mInterQueueSemaphores[i] = mDevice.getResourceManager().create<VulkanSemaphore>();
			semaphores[i + 1] = mInterQueueSemaphores[i]->getHandle();
		}

		mNumUsedInterQueueSemaphores = 0;
	}

	VulkanSemaphore* VulkanCmdBuffer::requestInterQueueSemaphore() const
	{
		if (mNumUsedInterQueueSemaphores >= BS_MAX_VULKAN_CB_DEPENDENCIES)
			return nullptr;

		return mInterQueueSemaphores[mNumUsedInterQueueSemaphores++];
	}

	void VulkanCmdBuffer::submit(VulkanQueue* queue, UINT32 queueIdx, UINT32 syncMask)
	{
		assert(isReadyForSubmit());

		// Make sure to reset the CB fence before we submit it
		VkResult result = vkResetFences(mDevice.getLogical(), 1, &mFence);
		assert(result == VK_SUCCESS);

		// If there are any query resets needed, execute those first
		VulkanDevice& device = queue->getDevice();
		if(!mQueuedQueryResets.empty())
		{
			VulkanCmdBuffer* cmdBuffer = device.getCmdBufferPool().getBuffer(mQueueFamily, false);
			VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();

			for (auto& entry : mQueuedQueryResets)
				entry->reset(vkCmdBuffer);

			cmdBuffer->end();
			queue->queueSubmit(cmdBuffer, nullptr, 0);

			mQueuedQueryResets.clear();
		}

		// Issue pipeline barriers for queue transitions (need to happen on original queue first, then on new queue)
		for (auto& entry : mBuffers)
		{
			VulkanBuffer* resource = static_cast<VulkanBuffer*>(entry.first);

			if (!resource->isExclusive())
				continue;

			UINT32 currentQueueFamily = resource->getQueueFamily();
			if (currentQueueFamily != (UINT32)-1 && currentQueueFamily != mQueueFamily)
			{
				Vector<VkBufferMemoryBarrier>& barriers = mTransitionInfoTemp[currentQueueFamily].bufferBarriers;

				barriers.push_back(VkBufferMemoryBarrier());
				VkBufferMemoryBarrier& barrier = barriers.back();
				barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = entry.second.accessFlags;
				barrier.dstAccessMask = entry.second.accessFlags;
				barrier.srcQueueFamilyIndex = currentQueueFamily;
				barrier.dstQueueFamilyIndex = mQueueFamily;
				barrier.buffer = resource->getHandle();
				barrier.offset = 0;
				barrier.size = VK_WHOLE_SIZE;
			}
		}

		// For images issue queue transitions, as above. Also issue layout transitions to their inital layouts.
		Vector<VkImageMemoryBarrier>& localBarriers = mTransitionInfoTemp[mQueueFamily].imageBarriers;
		for (auto& entry : mImages)
		{
			VulkanImage* resource = static_cast<VulkanImage*>(entry.first);
			ImageInfo& imageInfo = mImageInfos[entry.second];

			UINT32 currentQueueFamily = resource->getQueueFamily();
			bool queueMismatch = resource->isExclusive() && currentQueueFamily != (UINT32)-1
				&& currentQueueFamily != mQueueFamily;

			ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
			if (queueMismatch)
			{
				Vector<VkImageMemoryBarrier>& barriers = mTransitionInfoTemp[currentQueueFamily].imageBarriers;

				for (UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
				{
					ImageSubresourceInfo& subresourceInfo = subresourceInfos[i];

					UINT32 startIdx = (UINT32)barriers.size();
					resource->getBarriers(subresourceInfo.range, barriers);

					for(UINT32 j = startIdx; j < (UINT32)barriers.size(); j++)
					{
						VkImageMemoryBarrier& barrier = barriers[j];

						barrier.dstAccessMask = resource->getAccessFlags(barrier.oldLayout);
						barrier.newLayout = barrier.oldLayout;
						barrier.srcQueueFamilyIndex = currentQueueFamily;
						barrier.dstQueueFamilyIndex = mQueueFamily;
					}
				}
			}

			for (UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
			{
				ImageSubresourceInfo& subresourceInfo = subresourceInfos[i];

				VkImageLayout initialLayout = subresourceInfo.initialLayout;
				if (initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
					continue;

				const VkImageSubresourceRange& range = subresourceInfo.range;
				UINT32 mipEnd = range.baseMipLevel + range.levelCount;
				UINT32 faceEnd = range.baseArrayLayer + range.layerCount;
				if(!subresourceInfo.hasExternalTransition)
				{
					bool layoutMismatch = false;
					for (UINT32 mip = range.baseMipLevel; mip < mipEnd; mip++)
					{
						for (UINT32 face = range.baseArrayLayer; face < faceEnd; face++)
						{
							VulkanImageSubresource* subresource = resource->getSubresource(face, mip);
							if (subresource->getLayout() != initialLayout)
							{
								layoutMismatch = true;
								break;
							}
						}

						if (layoutMismatch)
							break;
					}

					if (layoutMismatch)
					{
						UINT32 startIdx = (UINT32)localBarriers.size();
						resource->getBarriers(subresourceInfo.range, localBarriers);

						for (UINT32 j = startIdx; j < (UINT32)localBarriers.size(); j++)
						{
							VkImageMemoryBarrier& barrier = localBarriers[j];

							barrier.dstAccessMask = resource->getAccessFlags(initialLayout, subresourceInfo.isInitialReadOnly);
							barrier.newLayout = initialLayout;
						}
					}
				}

				for (UINT32 mip = range.baseMipLevel; mip < mipEnd; mip++)
				{
					for (UINT32 face = range.baseArrayLayer; face < faceEnd; face++)
					{
						VulkanImageSubresource* subresource = resource->getSubresource(face, mip);
						subresource->setLayout(subresourceInfo.finalLayout);
					}
				}
			}
		}

		for (auto& entry : mTransitionInfoTemp)
		{
			bool empty = entry.second.imageBarriers.size() == 0 && entry.second.bufferBarriers.size() == 0;
			if (empty)
				continue;

			UINT32 entryQueueFamily = entry.first;

			// No queue transition needed for entries on this queue (this entry is most likely an image layout transition)
			if (entryQueueFamily == (UINT32)-1 || entryQueueFamily == mQueueFamily)
				continue;

			VulkanCmdBuffer* cmdBuffer = device.getCmdBufferPool().getBuffer(entryQueueFamily, false);
			VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();

			TransitionInfo& barriers = entry.second;
			UINT32 numImgBarriers = (UINT32)barriers.imageBarriers.size();
			UINT32 numBufferBarriers = (UINT32)barriers.bufferBarriers.size();

			VkPipelineStageFlags srcStage = 0;
			VkPipelineStageFlags dstStage = 0;
			getPipelineStageFlags(barriers.imageBarriers, srcStage, dstStage);

			vkCmdPipelineBarrier(vkCmdBuffer,
								 srcStage, dstStage,
								 0, 0, nullptr,
								 numBufferBarriers, barriers.bufferBarriers.data(),
								 numImgBarriers, barriers.imageBarriers.data());

			// Find an appropriate queue to execute on
			UINT32 otherQueueIdx = 0;
			VulkanQueue* otherQueue = nullptr;
			GpuQueueType otherQueueType = GQT_GRAPHICS;
			for (UINT32 i = 0; i < GQT_COUNT; i++)
			{
				otherQueueType = (GpuQueueType)i;
				if (device.getQueueFamily(otherQueueType) != entryQueueFamily)
					continue;

				UINT32 numQueues = device.getNumQueues(otherQueueType);
				for (UINT32 j = 0; j < numQueues; j++)
				{
					// Try to find a queue not currently executing
					VulkanQueue* curQueue = device.getQueue(otherQueueType, j);
					if (!curQueue->isExecuting())
					{
						otherQueue = curQueue;
						otherQueueIdx = j;
					}
				}

				// Can't find empty one, use the first one then
				if (otherQueue == nullptr)
				{
					otherQueue = device.getQueue(otherQueueType, 0);
					otherQueueIdx = 0;
				}

				break;
			}

			syncMask |= CommandSyncMask::getGlobalQueueMask(otherQueueType, otherQueueIdx);

			cmdBuffer->end();

			// Note: If I switch back to doing layout transitions here, I need to wait on present semaphore
			otherQueue->submit(cmdBuffer, nullptr, 0);
		}

		UINT32 deviceIdx = device.getIndex();
		VulkanCommandBufferManager& cbm = static_cast<VulkanCommandBufferManager&>(CommandBufferManager::instance());

		UINT32 numSemaphores;
		cbm.getSyncSemaphores(deviceIdx, syncMask, mSemaphoresTemp.data(), numSemaphores);

		// Wait on present (i.e. until the back buffer becomes available) for any swap chains
		for(auto& entry : mSwapChains)
		{
			const SwapChainSurface& surface = entry->getBackBuffer();
			if (surface.needsWait)
			{
				VulkanSemaphore* semaphore = entry->getBackBuffer().sync;

				if (numSemaphores >= (UINT32)mSemaphoresTemp.size())
					mSemaphoresTemp.push_back(semaphore);
				else
					mSemaphoresTemp[numSemaphores] = semaphore;

				numSemaphores++;

				entry->notifyBackBufferWaitIssued();
			}
		}

		// Issue second part of transition pipeline barriers (on this queue)
		for (auto& entry : mTransitionInfoTemp)
		{
			bool empty = entry.second.imageBarriers.size() == 0 && entry.second.bufferBarriers.size() == 0;
			if (empty)
				continue;

			VulkanCmdBuffer* cmdBuffer = device.getCmdBufferPool().getBuffer(mQueueFamily, false);
			VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();

			TransitionInfo& barriers = entry.second;
			UINT32 numImgBarriers = (UINT32)barriers.imageBarriers.size();
			UINT32 numBufferBarriers = (UINT32)barriers.bufferBarriers.size();

			VkPipelineStageFlags srcStage = 0;
			VkPipelineStageFlags dstStage = 0;
			getPipelineStageFlags(barriers.imageBarriers, srcStage, dstStage);

			vkCmdPipelineBarrier(vkCmdBuffer,
								 srcStage, dstStage,
								 0, 0, nullptr,
								 numBufferBarriers, barriers.bufferBarriers.data(),
								 numImgBarriers, barriers.imageBarriers.data());

			cmdBuffer->end();
			queue->queueSubmit(cmdBuffer, mSemaphoresTemp.data(), numSemaphores);

			numSemaphores = 0; // Semaphores are only needed the first time, since we're adding the buffers on the same queue
		}

		queue->queueSubmit(this, mSemaphoresTemp.data(), numSemaphores);
		queue->submitQueued();

		mGlobalQueueIdx = CommandSyncMask::getGlobalQueueIdx(queue->getType(), queueIdx);
		for (auto& entry : mResources)
		{
			ResourceUseHandle& useHandle = entry.second;
			assert(!useHandle.used);

			useHandle.used = true;
			entry.first->notifyUsed(mGlobalQueueIdx, mQueueFamily, useHandle.flags);
		}

		for (auto& entry : mImages)
		{
			UINT32 imageInfoIdx = entry.second;
			ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

			ResourceUseHandle& useHandle = imageInfo.useHandle;
			assert(!useHandle.used);

			useHandle.used = true;
			entry.first->notifyUsed(mGlobalQueueIdx, mQueueFamily, useHandle.flags);
		}

		for (auto& entry : mBuffers)
		{
			ResourceUseHandle& useHandle = entry.second.useHandle;
			assert(!useHandle.used);

			useHandle.used = true;
			entry.first->notifyUsed(mGlobalQueueIdx, mQueueFamily, useHandle.flags);
		}

		// Note: Uncomment for debugging only, prevents any device concurrency issues.
		// vkQueueWaitIdle(queue->getHandle());

		// Clear vectors but don't clear the actual map, as we want to re-use the memory since we expect queue family
		// indices to be the same
		for (auto& entry : mTransitionInfoTemp)
		{
			entry.second.imageBarriers.clear();
			entry.second.bufferBarriers.clear();
		}

		mGraphicsPipeline = nullptr;
		mComputePipeline = nullptr;
		mGfxPipelineRequiresBind = true;
		mCmpPipelineRequiresBind = true;
		mFramebuffer = nullptr;
		mDescriptorSetsBindState = DescriptorSetBindFlag::Graphics | DescriptorSetBindFlag::Compute;
		mQueuedLayoutTransitions.clear();
		mBoundParams = nullptr;
		mSwapChains.clear();
	}

	bool VulkanCmdBuffer::checkFenceStatus(bool block) const
	{
		VkResult result = vkWaitForFences(mDevice.getLogical(), 1, &mFence, true, block ? 1'000'000'000 : 0);
		assert(result == VK_SUCCESS || result == VK_TIMEOUT);

		return result == VK_SUCCESS;
	}

	void VulkanCmdBuffer::reset()
	{
		bool wasSubmitted = mState == State::Submitted;

		mState = State::Ready;
		vkResetCommandBuffer(mCmdBuffer, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT); // Note: Maybe better not to release resources?

		if (wasSubmitted)
		{
			for (auto& entry : mResources)
			{
				ResourceUseHandle& useHandle = entry.second;
				assert(useHandle.used);

				entry.first->notifyDone(mGlobalQueueIdx, useHandle.flags);
			}

			for (auto& entry : mImages)
			{
				UINT32 imageInfoIdx = entry.second;
				ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

				ResourceUseHandle& useHandle = imageInfo.useHandle;
				assert(useHandle.used);

				entry.first->notifyDone(mGlobalQueueIdx, useHandle.flags);
			}

			for (auto& entry : mBuffers)
			{
				ResourceUseHandle& useHandle = entry.second.useHandle;
				assert(useHandle.used);

				entry.first->notifyDone(mGlobalQueueIdx, useHandle.flags);
			}
		}
		else
		{
			for (auto& entry : mResources)
				entry.first->notifyUnbound();

			for (auto& entry : mImages)
				entry.first->notifyUnbound();

			for (auto& entry : mBuffers)
				entry.first->notifyUnbound();
		}

		mResources.clear();
		mImages.clear();
		mBuffers.clear();
		mOcclusionQueries.clear();
		mTimerQueries.clear();
		mImageInfos.clear();
		mSubresourceInfoStorage.clear();
		mPassTouchedSubresourceInfos.clear();
	}

	void VulkanCmdBuffer::setRenderTarget(const SPtr<RenderTarget>& rt, UINT32 readOnlyFlags, 
		RenderSurfaceMask loadMask)
	{
		assert(mState != State::Submitted);

		VulkanFramebuffer* newFB;
		if(rt != nullptr)
		{
			if (rt->getProperties().isWindow)
			{
#if BS_PLATFORM == BS_PLATFORM_WIN32
				Win32RenderWindow* window = static_cast<Win32RenderWindow*>(rt.get());
#elif BS_PLATFORM == BS_PLATFORM_LINUX
				LinuxRenderWindow* window = static_cast<LinuxRenderWindow*>(rt.get());
#elif BS_PLATFORM == BS_PLATFORM_OSX
                MacOSRenderWindow* window = static_cast<MacOSRenderWindow*>(rt.get());
#endif
				window->acquireBackBuffer();

				VulkanSwapChain* swapChain;
				rt->getCustomAttribute("SC", &swapChain);

				mSwapChains.insert(swapChain);
			}

			rt->getCustomAttribute("FB", &newFB);
		}
		else
		{
			newFB = nullptr;
		}

		// Warn if invalid load mask
		if (loadMask.isSet(RT_DEPTH) && !loadMask.isSet(RT_STENCIL))
		{
			LOGWRN("setRenderTarget() invalid load mask, depth enabled but stencil disabled. This is not supported. Both \
				will be loaded.");

			loadMask.set(RT_STENCIL);
		}

		if (!loadMask.isSet(RT_DEPTH) && loadMask.isSet(RT_STENCIL))
		{
			LOGWRN("setRenderTarget() invalid load mask, stencil enabled but depth disabled. This is not supported. Both \
				will be loaded.");

			loadMask.set(RT_DEPTH);
		}

		if (mFramebuffer == newFB && mRenderTargetReadOnlyFlags == readOnlyFlags && mRenderTargetLoadMask == loadMask)
			return;

		if (isInRenderPass())
			endRenderPass();
		else
		{
			// If a clear is queued for previous FB, execute the render pass with no additional instructions
			if (mClearMask)
				executeClearPass();
		}

		// Reset isFBAttachment flags for subresources from the old framebuffer
		if(mFramebuffer != nullptr)
		{
			UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
			for(UINT32 i = 0; i < numColorAttachments; i++)
			{
				const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getColorAttachment(i);
				UINT32 imageInfoIdx = mImages[fbAttachment.image];
				ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

				ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
				for(UINT32 j = 0; j < imageInfo.numSubresourceInfos; j++)
				{
					ImageSubresourceInfo& entry = subresourceInfos[j];
					entry.isFBAttachment = false;
				}
			}

			if(mFramebuffer->hasDepthAttachment())
			{
				const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getDepthStencilAttachment();
				UINT32 imageInfoIdx = mImages[fbAttachment.image];
				ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

				ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
				for(UINT32 j = 0; j < imageInfo.numSubresourceInfos; j++)
				{
					ImageSubresourceInfo& entry = subresourceInfos[j];
					entry.isFBAttachment = false;
				}
			}
		}

		if(newFB == nullptr)
		{
			mFramebuffer = nullptr;
			mRenderTargetReadOnlyFlags = 0;
			mRenderTargetLoadMask = RT_NONE;
		}
		else
		{
			mFramebuffer = newFB;
			mRenderTargetReadOnlyFlags = readOnlyFlags;
			mRenderTargetLoadMask = loadMask;
		}

		// Re-set the params as they will need to be re-bound
		setGpuParams(mBoundParams);

		if (mFramebuffer != nullptr)
			registerResource(mFramebuffer, loadMask, readOnlyFlags);

		mGfxPipelineRequiresBind = true;
	}

	void VulkanCmdBuffer::clearViewport(const Rect2I& area, UINT32 buffers, const Color& color, float depth, UINT16 stencil,
					   UINT8 targetMask)
	{
		if (buffers == 0 || mFramebuffer == nullptr)
			return;

		// Add clear command if currently in render pass
		if (isInRenderPass())
		{
			VkClearAttachment attachments[BS_MAX_MULTIPLE_RENDER_TARGETS + 1];
			UINT32 baseLayer = 0;

			UINT32 attachmentIdx = 0;
			if ((buffers & FBT_COLOR) != 0)
			{
				UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
				for (UINT32 i = 0; i < numColorAttachments; i++)
				{
					const VulkanFramebufferAttachment& attachment = mFramebuffer->getColorAttachment(i);

					if (((1 << attachment.index) & targetMask) == 0)
						continue;

					attachments[attachmentIdx].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					attachments[attachmentIdx].colorAttachment = i;

					VkClearColorValue& colorValue = attachments[attachmentIdx].clearValue.color;
					colorValue.float32[0] = color.r;
					colorValue.float32[1] = color.g;
					colorValue.float32[2] = color.b;
					colorValue.float32[3] = color.a;

					UINT32 curBaseLayer = attachment.baseLayer;
					if (attachmentIdx == 0)
						baseLayer = curBaseLayer;
					else
					{
						if (baseLayer != curBaseLayer)
						{
							// Note: This could be supported relatively easily: we would need to issue multiple separate
							// clear commands for such framebuffers. 
							LOGERR("Attempting to clear a texture that has multiple multi-layer surfaces with mismatching "
								   "starting layers. This is currently not supported.");
						}
					}

					attachmentIdx++;
				}
			}

			if ((buffers & FBT_DEPTH) != 0 || (buffers & FBT_STENCIL) != 0)
			{
				if (mFramebuffer->hasDepthAttachment())
				{
					attachments[attachmentIdx].aspectMask = 0;

					if ((buffers & FBT_DEPTH) != 0)
					{
						attachments[attachmentIdx].aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
						attachments[attachmentIdx].clearValue.depthStencil.depth = depth;
					}

					if ((buffers & FBT_STENCIL) != 0)
					{
						attachments[attachmentIdx].aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
						attachments[attachmentIdx].clearValue.depthStencil.stencil = stencil;
					}

					attachments[attachmentIdx].colorAttachment = 0;

					UINT32 curBaseLayer = mFramebuffer->getDepthStencilAttachment().baseLayer;
					if (attachmentIdx == 0)
						baseLayer = curBaseLayer;
					else
					{
						if (baseLayer != curBaseLayer)
						{
							// Note: This could be supported relatively easily: we would need to issue multiple separate
							// clear commands for such framebuffers. 
							LOGERR("Attempting to clear a texture that has multiple multi-layer surfaces with mismatching "
								   "starting layers. This is currently not supported.");
						}
					}

					attachmentIdx++;
				}
			}

			UINT32 numAttachments = attachmentIdx;
			if (numAttachments == 0)
				return;

			VkClearRect clearRect;
			clearRect.baseArrayLayer = baseLayer;
			clearRect.layerCount = mFramebuffer->getNumLayers();
			clearRect.rect.offset.x = area.x;
			clearRect.rect.offset.y = area.y;
			clearRect.rect.extent.width = area.width;
			clearRect.rect.extent.height = area.height;

			vkCmdClearAttachments(mCmdBuffer, numAttachments, attachments, 1, &clearRect);
		}
		// Otherwise we use a render pass that performs a clear on begin
		else
		{
			ClearMask clearMask;
			std::array<VkClearValue, BS_MAX_MULTIPLE_RENDER_TARGETS + 1> clearValues = mClearValues;

			UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
			if ((buffers & FBT_COLOR) != 0)
			{
				for (UINT32 i = 0; i < numColorAttachments; i++)
				{
					const VulkanFramebufferAttachment& attachment = mFramebuffer->getColorAttachment(i);

					if (((1 << attachment.index) & targetMask) == 0)
						continue;

					clearMask |= (ClearMaskBits)(1 << attachment.index);

					VkClearColorValue& colorValue = clearValues[i].color;
					colorValue.float32[0] = color.r;
					colorValue.float32[1] = color.g;
					colorValue.float32[2] = color.b;
					colorValue.float32[3] = color.a;
				}
			}

			if ((buffers & FBT_DEPTH) != 0 || (buffers & FBT_STENCIL) != 0)
			{
				if (mFramebuffer->hasDepthAttachment())
				{
					UINT32 depthAttachmentIdx = numColorAttachments;

					if ((buffers & FBT_DEPTH) != 0)
					{
						clearValues[depthAttachmentIdx].depthStencil.depth = depth;
						clearMask |= CLEAR_DEPTH;
					}

					if ((buffers & FBT_STENCIL) != 0)
					{
						clearValues[depthAttachmentIdx].depthStencil.stencil = stencil;
						clearMask |= CLEAR_STENCIL;
					}
				}
			}

			if (!clearMask)
				return;

			// Some previous clear operation is already queued, execute it first
			bool previousClearNeedsToFinish = (mClearMask & clearMask) != CLEAR_NONE;
			
			if(previousClearNeedsToFinish)
				executeClearPass();

			mClearMask |= clearMask;
			mClearValues = clearValues;
			mClearArea = area;
		}
	}

	void VulkanCmdBuffer::clearRenderTarget(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
	{
		Rect2I area(0, 0, mFramebuffer->getWidth(), mFramebuffer->getHeight());
		clearViewport(area, buffers, color, depth, stencil, targetMask);
	}

	void VulkanCmdBuffer::clearViewport(UINT32 buffers, const Color& color, float depth, UINT16 stencil, UINT8 targetMask)
	{
		Rect2I area;
		area.x = (UINT32)(mViewport.x * mFramebuffer->getWidth());
		area.y = (UINT32)(mViewport.y * mFramebuffer->getHeight());
		area.width = (UINT32)(mViewport.width * mFramebuffer->getWidth());
		area.height = (UINT32)(mViewport.height * mFramebuffer->getHeight());

		clearViewport(area, buffers, color, depth, stencil, targetMask);
	}

	void VulkanCmdBuffer::setPipelineState(const SPtr<GraphicsPipelineState>& state)
	{
		if (mGraphicsPipeline == state)
			return;

		mGraphicsPipeline = std::static_pointer_cast<VulkanGraphicsPipelineState>(state);
		mGfxPipelineRequiresBind = true; 
	}

	void VulkanCmdBuffer::setPipelineState(const SPtr<ComputePipelineState>& state)
	{
		if (mComputePipeline == state)
			return;

		mComputePipeline = std::static_pointer_cast<VulkanComputePipelineState>(state);
		mCmpPipelineRequiresBind = true;
	}

	void VulkanCmdBuffer::setGpuParams(const SPtr<GpuParams>& gpuParams)
	{
		// Note: We keep an internal reference to GPU params even though we shouldn't keep a reference to a core thread
		// object. But it should be fine since we expect the resource to be externally synchronized so it should never
		// be allowed to go out of scope on a non-core thread anyway.

		mBoundParams = std::static_pointer_cast<VulkanGpuParams>(gpuParams);
		
		if (mBoundParams != nullptr)
			mBoundParamsDirty = true;
		else
		{
			mNumBoundDescriptorSets = 0;
			mBoundParamsDirty = false;
		}

		mDescriptorSetsBindState = DescriptorSetBindFlag::Graphics | DescriptorSetBindFlag::Compute;
	}

	void VulkanCmdBuffer::setViewport(const Rect2& area)
	{
		if (mViewport == area)
			return;

		mViewport = area;
		mViewportRequiresBind = true;
	}

	void VulkanCmdBuffer::setScissorRect(const Rect2I& value)
	{
		if (mScissor == value)
			return;

		mScissor = value;
		mScissorRequiresBind = true;
	}

	void VulkanCmdBuffer::setStencilRef(UINT32 value)
	{
		if (mStencilRef == value)
			return;

		mStencilRef = value;
		mStencilRefRequiresBind = true;
	}

	void VulkanCmdBuffer::setDrawOp(DrawOperationType drawOp)
	{
		if (mDrawOp == drawOp)
			return;

		mDrawOp = drawOp;
		mGfxPipelineRequiresBind = true;
	}

	void VulkanCmdBuffer::setVertexBuffers(UINT32 index, SPtr<VertexBuffer>* buffers, UINT32 numBuffers)
	{
		if (numBuffers == 0)
			return;

		for(UINT32 i = 0; i < numBuffers; i++)
		{
			VulkanVertexBuffer* vertexBuffer = static_cast<VulkanVertexBuffer*>(buffers[i].get());

			if (vertexBuffer != nullptr)
			{
				VulkanBuffer* resource = vertexBuffer->getResource(mDevice.getIndex());
				if (resource != nullptr)
				{
					mVertexBuffersTemp[i] = resource->getHandle();

					registerResource(resource, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT, VulkanUseFlag::Read);
				}
				else
					mVertexBuffersTemp[i] = VK_NULL_HANDLE;
			}
			else
				mVertexBuffersTemp[i] = VK_NULL_HANDLE;
		}

		vkCmdBindVertexBuffers(mCmdBuffer, index, numBuffers, mVertexBuffersTemp, mVertexBufferOffsetsTemp);
	}

	void VulkanCmdBuffer::setIndexBuffer(const SPtr<IndexBuffer>& buffer)
	{
		VulkanIndexBuffer* indexBuffer = static_cast<VulkanIndexBuffer*>(buffer.get());

		VkBuffer vkBuffer = VK_NULL_HANDLE;
		VkIndexType indexType = VK_INDEX_TYPE_UINT32;
		if (indexBuffer != nullptr)
		{
			VulkanBuffer* resource = indexBuffer->getResource(mDevice.getIndex());
			if (resource != nullptr)
			{
				vkBuffer = resource->getHandle();
				indexType = VulkanUtility::getIndexType(buffer->getProperties().getType());

				registerResource(resource, VK_ACCESS_INDEX_READ_BIT, VulkanUseFlag::Read);
			}
		}

		vkCmdBindIndexBuffer(mCmdBuffer, vkBuffer, 0, indexType);
	}

	void VulkanCmdBuffer::setVertexDeclaration(const SPtr<VertexDeclaration>& decl)
	{
		if (mVertexDecl == decl)
			return;

		mVertexDecl = decl;
		mGfxPipelineRequiresBind = true;
	}

	bool VulkanCmdBuffer::isReadyForRender()
	{
		if (mGraphicsPipeline == nullptr)
			return false;

		SPtr<VertexDeclaration> inputDecl = mGraphicsPipeline->getInputDeclaration();
		if (inputDecl == nullptr)
			return false;

		return mFramebuffer != nullptr && mVertexDecl != nullptr;
	}

	bool VulkanCmdBuffer::bindGraphicsPipeline()
	{
		SPtr<VertexDeclaration> inputDecl = mGraphicsPipeline->getInputDeclaration();
		SPtr<VulkanVertexInput> vertexInput = VulkanVertexInputManager::instance().getVertexInfo(mVertexDecl, inputDecl);

		VulkanPipeline* pipeline = mGraphicsPipeline->getPipeline(mDevice.getIndex(), mFramebuffer,
			mRenderTargetReadOnlyFlags, mDrawOp, vertexInput);

		if (pipeline == nullptr)
			return false;

		// Check that pipeline matches the read-only state of any framebuffer attachments
		UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
		for (UINT32 i = 0; i < numColorAttachments; i++)
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getColorAttachment(i);
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face, 
				fbAttachment.surface.mipLevel);

			if (subresourceInfo.isShaderInput && !pipeline->isColorReadOnly(i))
			{
				LOGWRN("Framebuffer attachment also used as a shader input, but color writes aren't disabled. This will"
					" result in undefined behavior.");
			}
		}

		if (mFramebuffer->hasDepthAttachment())
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getDepthStencilAttachment();
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face,
				fbAttachment.surface.mipLevel);

			if (subresourceInfo.isShaderInput && !pipeline->isDepthReadOnly())
			{
				LOGWRN("Framebuffer attachment also used as a shader input, but depth/stencil writes aren't disabled. "
					"This will result in undefined behavior.");
			}
		}

		mGraphicsPipeline->registerPipelineResources(this);
		registerResource(pipeline, VulkanUseFlag::Read);

		vkCmdBindPipeline(mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->getHandle());
		bindDynamicStates(true);

		mGfxPipelineRequiresBind = false;
		return true;
	}

	void VulkanCmdBuffer::bindDynamicStates(bool forceAll)
	{
		if (mViewportRequiresBind || forceAll)
		{
			VkViewport viewport;
			viewport.x = mViewport.x * mFramebuffer->getWidth();
			viewport.y = mViewport.y * mFramebuffer->getHeight();
			viewport.width = mViewport.width * mFramebuffer->getWidth();
			viewport.height = mViewport.height * mFramebuffer->getHeight();
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;

			vkCmdSetViewport(mCmdBuffer, 0, 1, &viewport);
			mViewportRequiresBind = false;
		}

		if(mStencilRefRequiresBind || forceAll)
		{
			vkCmdSetStencilReference(mCmdBuffer, VK_STENCIL_FRONT_AND_BACK, mStencilRef);
			mStencilRefRequiresBind = false;
		}

		if(mScissorRequiresBind || forceAll)
		{
			VkRect2D scissorRect;
			if(mGraphicsPipeline->isScissorEnabled())
			{
				scissorRect.offset.x = mScissor.x;
				scissorRect.offset.y = mScissor.y;
				scissorRect.extent.width = mScissor.width;
				scissorRect.extent.height = mScissor.height;
			}
			else
			{
				scissorRect.offset.x = 0;
				scissorRect.offset.y = 0;
				scissorRect.extent.width = mFramebuffer->getWidth();
				scissorRect.extent.height = mFramebuffer->getHeight();
			}

			vkCmdSetScissor(mCmdBuffer, 0, 1, &scissorRect);

			mScissorRequiresBind = false;
		}
	}

	void VulkanCmdBuffer::bindGpuParams()
	{
		if (mBoundParamsDirty)
		{
			if (mBoundParams != nullptr)
			{
				mNumBoundDescriptorSets = mBoundParams->getNumSets();
				mBoundParams->prepareForBind(*this, mDescriptorSetsTemp);
			}
			else
				mNumBoundDescriptorSets = 0;

			mBoundParamsDirty = false;
		}
		else
		{
			mNumBoundDescriptorSets = 0;
		}
	}

	void VulkanCmdBuffer::executeLayoutTransitions()
	{
		auto createLayoutTransitionBarrier = [&](VulkanImage* image, ImageInfo& imageInfo)
		{
			ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
			for (UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
			{
				ImageSubresourceInfo& subresourceInfo = subresourceInfos[i];

				if (!subresourceInfo.hasTransitioned || subresourceInfo.currentLayout == subresourceInfo.requiredLayout)
					continue;

				mLayoutTransitionBarriersTemp.push_back(VkImageMemoryBarrier());
				VkImageMemoryBarrier& barrier = mLayoutTransitionBarriersTemp.back();
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				barrier.pNext = nullptr;
				barrier.srcAccessMask = image->getAccessFlags(subresourceInfo.currentLayout);
				barrier.dstAccessMask = image->getAccessFlags(subresourceInfo.requiredLayout, subresourceInfo.isReadOnly);
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.oldLayout = subresourceInfo.currentLayout;
				barrier.newLayout = subresourceInfo.requiredLayout;
				barrier.image = image->getHandle();
				barrier.subresourceRange = subresourceInfo.range;

				subresourceInfo.currentLayout = subresourceInfo.requiredLayout;
				subresourceInfo.isReadOnly = true;
				subresourceInfo.hasTransitioned = true;
			}
		};

		// Note: These layout transitions will contain transitions for offscreen framebuffer attachments (while they 
		// transition to shader read-only layout). This can be avoided, since they're immediately used by the render pass
		// as color attachments, making the layout change redundant.
		for (auto& entry : mQueuedLayoutTransitions)
		{
			UINT32 imageInfoIdx = entry.second;
			ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

			createLayoutTransitionBarrier(entry.first, imageInfo);
		}

		VkPipelineStageFlags srcStage = 0;
		VkPipelineStageFlags dstStage = 0;
		getPipelineStageFlags(mLayoutTransitionBarriersTemp, srcStage, dstStage);

		vkCmdPipelineBarrier(mCmdBuffer,
							 srcStage, dstStage,
							 0, 0, nullptr,
							 0, nullptr,
							 (UINT32)mLayoutTransitionBarriersTemp.size(), mLayoutTransitionBarriersTemp.data());

		mQueuedLayoutTransitions.clear();
		mLayoutTransitionBarriersTemp.clear();
	}

	void VulkanCmdBuffer::updateFinalLayouts()
	{
		if (mFramebuffer == nullptr)
			return;

		UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
		for (UINT32 i = 0; i < numColorAttachments; i++)
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getColorAttachment(i);
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face,
																		fbAttachment.surface.mipLevel);

			subresourceInfo.currentLayout = subresourceInfo.finalLayout;
			subresourceInfo.requiredLayout = subresourceInfo.finalLayout;
			subresourceInfo.hasTransitioned = true;
		}

		if (mFramebuffer->hasDepthAttachment())
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getDepthStencilAttachment();
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face,
																		fbAttachment.surface.mipLevel);

			subresourceInfo.currentLayout = subresourceInfo.finalLayout;
			subresourceInfo.requiredLayout = subresourceInfo.finalLayout;
			subresourceInfo.hasTransitioned = true;
		}
	}

	void VulkanCmdBuffer::executeClearPass()
	{
		assert(mState == State::Recording);

		executeLayoutTransitions();

		VkRenderPassBeginInfo renderPassBeginInfo;
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext = nullptr;
		renderPassBeginInfo.framebuffer = mFramebuffer->getFramebuffer(RT_NONE, RT_NONE, mClearMask);
		renderPassBeginInfo.renderPass = mFramebuffer->getRenderPass(RT_NONE, RT_NONE, mClearMask);
		renderPassBeginInfo.renderArea.offset.x = mClearArea.x;
		renderPassBeginInfo.renderArea.offset.y = mClearArea.y;
		renderPassBeginInfo.renderArea.extent.width = mClearArea.width;
		renderPassBeginInfo.renderArea.extent.height = mClearArea.height;
		renderPassBeginInfo.clearValueCount = mFramebuffer->getNumClearEntries(mClearMask);
		renderPassBeginInfo.pClearValues = mClearValues.data();

		vkCmdBeginRenderPass(mCmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdEndRenderPass(mCmdBuffer);

		updateFinalLayouts();

		mClearMask = CLEAR_NONE;
	}

	void VulkanCmdBuffer::draw(UINT32 vertexOffset, UINT32 vertexCount, UINT32 instanceCount)
	{
		if (!isReadyForRender())
			return;

		// Need to bind gpu params before starting render pass, in order to make sure any layout transitions execute
		bindGpuParams();

		if (!isInRenderPass())
			beginRenderPass();

		if (mGfxPipelineRequiresBind)
		{
			if (!bindGraphicsPipeline())
				return;
		}
		else
			bindDynamicStates(false);

		if (mDescriptorSetsBindState.isSet(DescriptorSetBindFlag::Graphics))
		{
			if (mNumBoundDescriptorSets > 0)
			{
				UINT32 deviceIdx = mDevice.getIndex();
				VkPipelineLayout pipelineLayout = mGraphicsPipeline->getPipelineLayout(deviceIdx);

				vkCmdBindDescriptorSets(mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
										mNumBoundDescriptorSets, mDescriptorSetsTemp, 0, nullptr);
			}

			mDescriptorSetsBindState.unset(DescriptorSetBindFlag::Graphics);
		}

		if (instanceCount <= 0)
			instanceCount = 1;

		vkCmdDraw(mCmdBuffer, vertexCount, instanceCount, vertexOffset, 0);
	}

	void VulkanCmdBuffer::drawIndexed(UINT32 startIndex, UINT32 indexCount, UINT32 vertexOffset, UINT32 instanceCount)
	{
		if (!isReadyForRender())
			return;

		// Need to bind gpu params before starting render pass, in order to make sure any layout transitions execute
		bindGpuParams();

		if (!isInRenderPass())
			beginRenderPass();

		if (mGfxPipelineRequiresBind)
		{
			if (!bindGraphicsPipeline())
				return;
		}
		else
			bindDynamicStates(false);

		if (mDescriptorSetsBindState.isSet(DescriptorSetBindFlag::Graphics))
		{
			if (mNumBoundDescriptorSets > 0)
			{
				UINT32 deviceIdx = mDevice.getIndex();
				VkPipelineLayout pipelineLayout = mGraphicsPipeline->getPipelineLayout(deviceIdx);

				vkCmdBindDescriptorSets(mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
										mNumBoundDescriptorSets, mDescriptorSetsTemp, 0, nullptr);
			}

			mDescriptorSetsBindState.unset(DescriptorSetBindFlag::Graphics);
		}

		if (instanceCount <= 0)
			instanceCount = 1;

		vkCmdDrawIndexed(mCmdBuffer, indexCount, instanceCount, startIndex, vertexOffset, 0);
	}

	void VulkanCmdBuffer::dispatch(UINT32 numGroupsX, UINT32 numGroupsY, UINT32 numGroupsZ)
	{
		if (mComputePipeline == nullptr)
			return;

		if (isInRenderPass())
			endRenderPass();

		// Note: Should I restore the render target after? Note that this is only being done is framebuffer subresources
		// have their "isFBAttachment" flag reset, potentially I can just clear/restore those
		setRenderTarget(nullptr, 0, RT_ALL);

		// Need to bind gpu params before starting render pass, in order to make sure any layout transitions execute
		bindGpuParams();
		executeLayoutTransitions();

		UINT32 deviceIdx = mDevice.getIndex();
		if(mCmpPipelineRequiresBind)
		{
			VulkanPipeline* pipeline = mComputePipeline->getPipeline(deviceIdx);
			if (pipeline == nullptr)
				return;

			registerResource(pipeline, VulkanUseFlag::Read);
			mComputePipeline->registerPipelineResources(this);

			vkCmdBindPipeline(mCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline->getHandle());
			mCmpPipelineRequiresBind = false;
		}

		if(mDescriptorSetsBindState.isSet(DescriptorSetBindFlag::Compute))
		{
			if (mNumBoundDescriptorSets > 0)
			{
				VkPipelineLayout pipelineLayout = mComputePipeline->getPipelineLayout(deviceIdx);
				vkCmdBindDescriptorSets(mCmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0,
										mNumBoundDescriptorSets, mDescriptorSetsTemp, 0, nullptr);
			}

			mDescriptorSetsBindState.unset(DescriptorSetBindFlag::Compute);
		}

		vkCmdDispatch(mCmdBuffer, numGroupsX, numGroupsY, numGroupsZ);

		// Update any layout transitions that were performed by subpass dependencies, reset flags that signal image usage
		// and reset read-only state.
		for(auto& entry : mPassTouchedSubresourceInfos)
		{
			ImageSubresourceInfo& subresourceInfo = mSubresourceInfoStorage[entry];
			subresourceInfo.isShaderInput = false;
			subresourceInfo.isReadOnly = true;
			subresourceInfo.needsBarrier = false;
		}

		for (auto& entry : mBuffers)
			entry.second.needsBarrier = false;

		mPassTouchedSubresourceInfos.clear();
	}

	void VulkanCmdBuffer::setEvent(VulkanEvent* event)
	{
		if(isInRenderPass())
			mQueuedEvents.push_back(event);
		else
			vkCmdSetEvent(mCmdBuffer, event->getHandle(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
	}

	void VulkanCmdBuffer::resetQuery(VulkanQuery* query)
	{
		if (isInRenderPass())
			mQueuedQueryResets.push_back(query);
		else
			query->reset(mCmdBuffer);
	}

	void VulkanCmdBuffer::memoryBarrier(VkBuffer buffer, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags,
											 VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage)
	{
		VkBufferMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = srcAccessFlags;
		barrier.dstAccessMask = dstAccessFlags;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = buffer;
		barrier.offset = 0;
		barrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(getHandle(),
							 srcStage,
							 dstStage,
							 0, 0, nullptr,
							 1, &barrier,
							 0, nullptr);
	}

	void VulkanCmdBuffer::memoryBarrier(VkImage image, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags,
										VkPipelineStageFlags srcStage, VkPipelineStageFlags dstStage, VkImageLayout layout,
										const VkImageSubresourceRange& range)
	{
		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = srcAccessFlags;
		barrier.dstAccessMask = dstAccessFlags;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange = range;
		barrier.oldLayout = layout;
		barrier.newLayout = layout;

		vkCmdPipelineBarrier(getHandle(),
							 srcStage,
							 dstStage,
							 0, 0, nullptr,
							 0, nullptr,
							 1, &barrier);
	}

	void VulkanCmdBuffer::setLayout(VkImage image, VkAccessFlags srcAccessFlags, VkAccessFlags dstAccessFlags,
										 VkImageLayout oldLayout, VkImageLayout newLayout, const VkImageSubresourceRange& range)
	{
		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.pNext = nullptr;
		barrier.srcAccessMask = srcAccessFlags;
		barrier.dstAccessMask = dstAccessFlags;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.image = image;
		barrier.subresourceRange = range;

		VkPipelineStageFlags srcStage = getPipelineStageFlags(srcAccessFlags);
		VkPipelineStageFlags dstStage = getPipelineStageFlags(dstAccessFlags);
		
		vkCmdPipelineBarrier(getHandle(),
							 srcStage,
							 dstStage,
							 0, 0, nullptr,
							 0, nullptr,
							 1, &barrier);
	}

	VkImageLayout VulkanCmdBuffer::getCurrentLayout(VulkanImage* image, const VkImageSubresourceRange& range, 
		bool inRenderPass)
	{
		UINT32 face = range.baseArrayLayer;
		UINT32 mip = range.baseMipLevel;

		// The assumption is that all the subresources in the range will have the same layout, as this should be handled
		// by registerResource(), or by external code (in the case of transfers). So we only check the first subresource.
		VulkanImageSubresource* subresource = image->getSubresource(face, mip);

		auto iterFind = mImages.find(image);
		if (iterFind == mImages.end())
			return subresource->getLayout();

		UINT32 imageInfoIdx = iterFind->second;
		ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

		ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
		for(UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
		{
			ImageSubresourceInfo& entry = subresourceInfos[i];
			if(face >= entry.range.baseArrayLayer && face < (entry.range.baseArrayLayer + entry.range.layerCount) &&
			   mip >= entry.range.baseMipLevel && mip < (entry.range.baseMipLevel + entry.range.levelCount))
			{
				// If it's a FB attachment, retrieve its layout after the render pass begins
				if(entry.isFBAttachment && inRenderPass && mFramebuffer)
				{
					RenderSurfaceMask readMask = getFBReadMask();

					// Is it a depth-stencil attachment?
					if(mFramebuffer->hasDepthAttachment() && mFramebuffer->getDepthStencilAttachment().image == image)
					{
						if (readMask.isSet(RT_DEPTH))
						{
							if (readMask.isSet(RT_STENCIL))
								return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
							else // Depth readable but stencil isn't
								return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR;
						}
						else
						{
							if (readMask.isSet(RT_STENCIL)) // Stencil readable but depth isn't
								return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR;
							else 
								return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
							
						}
					}
					else // It is a color attachment
					{
						UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
						for (UINT32 j = 0; j < numColorAttachments; j++)
						{
							const VulkanFramebufferAttachment& attachment = mFramebuffer->getColorAttachment(j);

							if (attachment.image == image)
							{
								if (readMask.isSet((RenderSurfaceMaskBits)(1 << attachment.index)))
									return VK_IMAGE_LAYOUT_GENERAL;
								else
									return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
							}
						}
					}
				}

				return entry.requiredLayout;
			}
		}

		return subresource->getLayout();
	}

	void VulkanCmdBuffer::registerResource(VulkanResource* res, VulkanUseFlags flags)
	{
		auto insertResult = mResources.insert(std::make_pair(res, ResourceUseHandle()));
		if(insertResult.second) // New element
		{
			ResourceUseHandle& useHandle = insertResult.first->second;
			useHandle.used = false;
			useHandle.flags = flags;

			res->notifyBound();
		}
		else // Existing element
		{
			ResourceUseHandle& useHandle = insertResult.first->second;

			assert(!useHandle.used);
			useHandle.flags |= flags;
		}
	}

	void VulkanCmdBuffer::registerResource(VulkanImage* res, const VkImageSubresourceRange& range, VulkanUseFlags flags,
		ResourceUsage usage)
	{
		VkImageLayout layout = res->getOptimalLayout();

		registerResource(res, range, layout, layout, flags, usage);
	}

	void VulkanCmdBuffer::registerResource(VulkanImage* res, const VkImageSubresourceRange& range, 
		VkImageLayout newLayout, VkImageLayout finalLayout, VulkanUseFlags flags, ResourceUsage usage)
	{
		bool isFBAttachment = usage == ResourceUsage::Framebuffer;
		bool isTransfer = usage == ResourceUsage::Transfer;
		bool isShaderBind = usage == ResourceUsage::ShaderBind;

		// If binding it for write in a shader (not as color attachment or transfer op), we will need to issue a memory
		// barrier if the image gets used again during this render pass, so remember this information.
		bool needsBarrier = isShaderBind && flags.isSet(VulkanUseFlag::Write);

		UINT32 nextImageInfoIdx = (UINT32)mImageInfos.size();
		auto registerSubresourceInfo = [&](const VkImageSubresourceRange& subresourceRange)
		{
			mSubresourceInfoStorage.push_back(ImageSubresourceInfo());
			ImageSubresourceInfo& subresourceInfo = mSubresourceInfoStorage.back();
			subresourceInfo.currentLayout = newLayout;
			subresourceInfo.initialLayout = newLayout;
			subresourceInfo.requiredLayout = newLayout;
			subresourceInfo.finalLayout = finalLayout;
			subresourceInfo.range = subresourceRange;
			subresourceInfo.isFBAttachment = isFBAttachment;
			subresourceInfo.isShaderInput = isShaderBind;

			if (!isTransfer)
			{
				subresourceInfo.hasExternalTransition = false;
				subresourceInfo.hasTransitioned = false;
				subresourceInfo.isReadOnly = !flags.isSet(VulkanUseFlag::Write);
			}
			else
			{ 
				// Transfers handle layout transitions externally (at this point they are assumed to already be done) 
				subresourceInfo.hasExternalTransition = true;
				subresourceInfo.hasTransitioned = true;
				subresourceInfo.isReadOnly = true; // Doesn't matter for transfers
			}

			subresourceInfo.isInitialReadOnly = subresourceInfo.isReadOnly;
			subresourceInfo.needsBarrier = needsBarrier;

			mPassTouchedSubresourceInfos.insert((UINT32)mSubresourceInfoStorage.size() - 1);
		};

		auto insertResult = mImages.insert(std::make_pair(res, nextImageInfoIdx));
		if (insertResult.second) // New element
		{
			UINT32 imageInfoIdx = insertResult.first->second;
			mImageInfos.push_back(ImageInfo());

			ImageInfo& imageInfo = mImageInfos[imageInfoIdx];
			imageInfo.subresourceInfoIdx = (UINT32)mSubresourceInfoStorage.size();
			imageInfo.numSubresourceInfos = 1;

			imageInfo.useHandle.used = false;
			imageInfo.useHandle.flags = flags;

			registerSubresourceInfo(range);

			res->notifyBound();
		}
		else // Existing element
		{
			UINT32 imageInfoIdx = insertResult.first->second;
			ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

			assert(!imageInfo.useHandle.used);
			imageInfo.useHandle.flags |= flags;

			// See if there is an overlap between existing ranges and the new range. And if so break them up accordingly.
			//// First test for the simplest and most common case (same range or no overlap) to avoid more complex
			//// computations.
			ImageSubresourceInfo* subresources = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
			
			bool foundRange = false;
			for(UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
			{
				if(VulkanUtility::rangeOverlaps(subresources[i].range, range))
				{
					if (subresources[i].range.layerCount == range.layerCount &&
						subresources[i].range.levelCount == range.levelCount &&
						subresources[i].range.baseArrayLayer == range.baseArrayLayer &&
						subresources[i].range.baseMipLevel == range.baseMipLevel)
					{
						// Just update existing range
						updateSubresourceInfo(res, imageInfoIdx, subresources[i], newLayout, finalLayout, flags, usage);
						mPassTouchedSubresourceInfos.insert(imageInfo.subresourceInfoIdx + i);

						foundRange = true;
						break;
					}

					break;
				}
			}

			//// We'll need to update subresource ranges or add new ones. The hope is that this code is trigger VERY rarely
			//// (for just a few specific textures per frame).
			if (!foundRange)
			{
				std::array<VkImageSubresourceRange, 5> tempCutRanges;

				bs_frame_mark();
				{
					// We orphan previously allocated memory (we reset it after submit() anyway)
					UINT32 newSubresourceIdx = (UINT32)mSubresourceInfoStorage.size();

					FrameVector<UINT32> cutOverlappingRanges;
					for (UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
					{
						UINT32 subresourceIdx = imageInfo.subresourceInfoIdx + i;
						ImageSubresourceInfo& subresource = mSubresourceInfoStorage[subresourceIdx];

						if (!VulkanUtility::rangeOverlaps(subresource.range, range))
						{
							// Just copy as is
							mSubresourceInfoStorage.push_back(subresource);
							mPassTouchedSubresourceInfos.insert((UINT32)mSubresourceInfoStorage.size() - 1);
						}
						else // Need to cut
						{
							UINT32 numCutRanges;
							VulkanUtility::cutRange(subresource.range, range, tempCutRanges, numCutRanges);

							for(UINT32 j = 0; j < numCutRanges; j++)
							{
								// Create a copy of the original subresource with the new range
								ImageSubresourceInfo newInfo = mSubresourceInfoStorage[subresourceIdx];
								newInfo.range = tempCutRanges[j];

								if(VulkanUtility::rangeOverlaps(tempCutRanges[j], range))
								{
									// Update overlapping sub-resource range with new data from this range
									updateSubresourceInfo(res, imageInfoIdx, newInfo, newLayout, finalLayout, flags, usage);

									// Keep track of the overlapping ranges for later
									cutOverlappingRanges.push_back((UINT32)mSubresourceInfoStorage.size());
								}

								mSubresourceInfoStorage.push_back(newInfo);
								mPassTouchedSubresourceInfos.insert((UINT32)mSubresourceInfoStorage.size() - 1);
							}
						}
					}

					// Our range doesn't overlap with any existing ranges, so just add it
					if(cutOverlappingRanges.size() == 0)
					{
						registerSubresourceInfo(range);
					}
					else // Search if overlapping ranges fully cover the requested range, and insert non-covered regions
					{
						FrameQueue<VkImageSubresourceRange> sourceRanges;
						sourceRanges.push(range);

						for(auto& entry : cutOverlappingRanges)
						{
							VkImageSubresourceRange& overlappingRange = mSubresourceInfoStorage[entry].range;

							UINT32 numSourceRanges = (UINT32)sourceRanges.size();
							for(UINT32 i = 0; i < numSourceRanges; i++)
							{
								VkImageSubresourceRange sourceRange = sourceRanges.front();
								sourceRanges.pop();

								UINT32 numCutRanges;
								VulkanUtility::cutRange(sourceRange, overlappingRange, tempCutRanges, numCutRanges);

								for(UINT32 j = 0; j < numCutRanges; j++)
								{
									// We only care about ranges outside of the ones we already covered
									if(!VulkanUtility::rangeOverlaps(tempCutRanges[j], overlappingRange))
										sourceRanges.push(tempCutRanges[j]);
								}
							}
						}

						// Any remaining range hasn't been covered yet
						while(!sourceRanges.empty())
						{
							registerSubresourceInfo(sourceRanges.front());
							sourceRanges.pop();
						}
					}

					imageInfo.subresourceInfoIdx = newSubresourceIdx;
					imageInfo.numSubresourceInfos = (UINT32)mSubresourceInfoStorage.size() - newSubresourceIdx;
				}
				bs_frame_clear();
			}
		}

		// Register any sub-resources
		for(UINT32 i = 0; i < range.layerCount; i++)
		{
			for(UINT32 j = 0; j < range.levelCount; j++)
			{
				UINT32 layer = range.baseArrayLayer + i;
				UINT32 mipLevel = range.baseMipLevel + j;

				registerResource(res->getSubresource(layer, mipLevel), flags);
			}
		}
	}

	void VulkanCmdBuffer::registerResource(VulkanBuffer* res, VkAccessFlags accessFlags, VulkanUseFlags flags)
	{
		bool isShaderWrite = (accessFlags & VK_ACCESS_SHADER_WRITE_BIT) != 0;

		auto insertResult = mBuffers.insert(std::make_pair(res, BufferInfo()));
		if (insertResult.second) // New element
		{
			BufferInfo& bufferInfo = insertResult.first->second;
			bufferInfo.accessFlags = accessFlags;

			bufferInfo.useHandle.used = false;
			bufferInfo.useHandle.flags = flags;

			// Any writes done on storage buffers will need explicit memory barriers (if read during the same pass) so
			// we remember this, in case this buffers gets used later in this pass.
			bufferInfo.needsBarrier = isShaderWrite;

			res->notifyBound();
		}
		else // Existing element
		{
			BufferInfo& bufferInfo = insertResult.first->second;

			assert(!bufferInfo.useHandle.used);
			bufferInfo.useHandle.flags |= flags;
			bufferInfo.accessFlags |= accessFlags;

			// If the buffer was written to previously in this pass, and is now being used by a shader we need to issue
			// a barrier to make those writes visible. (Currently only generic, vertex and index buffers can be bound for
			// shader writes, so we only check their access flags).
			bool isShaderRead = (accessFlags & (VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INDEX_READ_BIT | 
				VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT)) != 0;
			if(bufferInfo.needsBarrier && (isShaderRead || isShaderWrite))
			{
				// Need to end render pass in order to execute the barrier. Hopefully this won't trigger much since most
				// shader writes are done during compute
				if (isInRenderPass())
					endRenderPass();

				VkPipelineStageFlags stages =
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
					VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
					VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
					VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

				VkBuffer buffer = res->getHandle();
				memoryBarrier(buffer, VK_ACCESS_SHADER_WRITE_BIT, accessFlags, stages, stages);

				bufferInfo.needsBarrier = isShaderWrite;
			}
		}
	}

	void VulkanCmdBuffer::registerResource(VulkanFramebuffer* res, RenderSurfaceMask loadMask, UINT32 readMask)
	{
		auto insertResult = mResources.insert(std::make_pair(res, ResourceUseHandle()));
		if (insertResult.second) // New element
		{
			ResourceUseHandle& useHandle = insertResult.first->second;
			useHandle.used = false;
			useHandle.flags = VulkanUseFlag::Write;

			res->notifyBound();
		}
		else // Existing element
		{
			ResourceUseHandle& useHandle = insertResult.first->second;

			assert(!useHandle.used);
			useHandle.flags |= VulkanUseFlag::Write;
		}

		// Register any sub-resources
		UINT32 numColorAttachments = res->getNumColorAttachments();
		for (UINT32 i = 0; i < numColorAttachments; i++)
		{
			const VulkanFramebufferAttachment& attachment = res->getColorAttachment(i);

			// If image is being loaded, we need to transfer it to correct layout, otherwise it doesn't matter
			VkImageLayout layout;
			if (loadMask.isSet((RenderSurfaceMaskBits)(1 << i)))
			{
				// Note that this might not be the actual layout used during the render pass, as the render pass can
				// transition this to a different layout when it begins, but we handle that elsewhere
				layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else
				layout = VK_IMAGE_LAYOUT_UNDEFINED;

			VkImageSubresourceRange range = attachment.image->getRange(attachment.surface);
			registerResource(attachment.image, range, layout, attachment.finalLayout, VulkanUseFlag::Write, 
				ResourceUsage::Framebuffer);
		}

		if(res->hasDepthAttachment())
		{
			const VulkanFramebufferAttachment& attachment = res->getDepthStencilAttachment();
			VulkanUseFlag useFlag = VulkanUseFlag::Write;

			// If image is being loaded, we need to transfer it to correct layout, otherwise it doesn't matter
			VkImageLayout layout;
			if (loadMask.isSet(RT_DEPTH) || loadMask.isSet(RT_STENCIL)) // Can't load one without the other
			{
				// Note that this might not be the actual layout used during the render pass, as the render pass can
				// transition this to a different layout when it begins, but we handle that elsewhere
				layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else
				layout = VK_IMAGE_LAYOUT_UNDEFINED;

			VkImageSubresourceRange range = attachment.image->getRange(attachment.surface);
			registerResource(attachment.image, range, layout, attachment.finalLayout, useFlag, ResourceUsage::Framebuffer);
		}
	}

	bool VulkanCmdBuffer::updateSubresourceInfo(VulkanImage* image, UINT32 imageInfoIdx, 
			ImageSubresourceInfo& subresourceInfo, VkImageLayout newLayout, VkImageLayout finalLayout, VulkanUseFlags flags, 
			ResourceUsage usage)
	{
		bool isTransfer = usage == ResourceUsage::Transfer;
		bool isFBAttachment = usage == ResourceUsage::Framebuffer;
		bool isShaderBind = usage == ResourceUsage::ShaderBind;

		// Transfers are always considered read only because this only matters for destination access mask, and transfers
		// handle that externally
		if(!isTransfer)
			subresourceInfo.isReadOnly &= !flags.isSet(VulkanUseFlag::Write);

		// For transfers, just assign new layout, external code does all the work
		if (isTransfer)
			subresourceInfo.requiredLayout = newLayout;
		else
		{
			// New layout is valid, check for transitions (UNDEFINED signifies the caller doesn't want a layout transition)
			if (newLayout != VK_IMAGE_LAYOUT_UNDEFINED)
			{
				// If layout transition was requested by framebuffer bind, respect it because render-pass will only accept a
				// specific layout (in certain cases), and we have no choice.
				// In the case when a FB attachment is also bound for shader reads, this will override the layout required for
				// shader read (GENERAL or DEPTH_READ_ONLY), but that is fine because those transitions are handled
				// automatically by render-pass layout transitions.
				// Any other texture (non FB attachment) will only even be bound in a single layout and we can keep the one it
				// was originally registered with.
				if (isFBAttachment)
					subresourceInfo.requiredLayout = newLayout;
				else if (!subresourceInfo.isFBAttachment) // Layout transition is not being done on a FB image
				{
					// Check if the image had a layout previously assigned, and if so check if multiple different layouts
					// were requested. In that case we wish to transfer the image to GENERAL layout.

					bool firstUseInRenderPass = !subresourceInfo.isShaderInput && !subresourceInfo.isFBAttachment;
					if (firstUseInRenderPass || subresourceInfo.requiredLayout == VK_IMAGE_LAYOUT_UNDEFINED)
						subresourceInfo.requiredLayout = newLayout;
					else if (subresourceInfo.requiredLayout != newLayout)
						subresourceInfo.requiredLayout = VK_IMAGE_LAYOUT_GENERAL;
				}
			}
		}

		// If attached to FB, then the final layout is set by the FB (provided as layout param here), otherwise its
		// the same as required layout
		if (!isFBAttachment && !subresourceInfo.isFBAttachment)
			subresourceInfo.finalLayout = subresourceInfo.requiredLayout;
		else
		{
			if (isFBAttachment)
				subresourceInfo.finalLayout = finalLayout;
		}

		// Queue layout transition, if not transfer. Transfer handle layout transitions externally.
		if (!isTransfer)
		{
			// If we haven't done a layout transition yet, we can just overwrite the previously written values, and the
			// transition will be handled as the first thing in submit(), otherwise we queue a non-initial transition
			// below.
			if (!subresourceInfo.hasTransitioned)
			{
				subresourceInfo.initialLayout = subresourceInfo.requiredLayout;
				subresourceInfo.currentLayout = subresourceInfo.requiredLayout;
				subresourceInfo.isInitialReadOnly = subresourceInfo.isReadOnly;
			}
			else
			{
				if (subresourceInfo.currentLayout != subresourceInfo.requiredLayout)
					mQueuedLayoutTransitions[image] = imageInfoIdx;
			}
		}
		else
		{
			// Resource has already transitioned its layout externally since this is a transfer. We cannot allow a
			// transition in submit().
			subresourceInfo.currentLayout = subresourceInfo.requiredLayout;
			subresourceInfo.hasTransitioned = true;
		}

		// If a FB attachment was just bound as a shader input, we might need to restart the render pass with a FB
		// attachment that supports read-only attachments using the GENERAL or DEPTH_READ_ONLY layout
		bool resetRenderPass = false;
		if (isFBAttachment)
		{
			if (!subresourceInfo.isFBAttachment)
			{
				subresourceInfo.isFBAttachment = true;
				resetRenderPass = subresourceInfo.isShaderInput;
			}
		}
		else
		{
			if (!subresourceInfo.isShaderInput)
			{
				subresourceInfo.isShaderInput = true;

				if (subresourceInfo.isFBAttachment)
				{
					// Special case for depth: If user has set up proper read-only flags, then the render pass will have
					// taken care of setting the valid state anyway, so no need to end the render pass
					if (subresourceInfo.requiredLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
					{
						if ((mRenderTargetReadOnlyFlags & FBT_DEPTH) != 0 || (mRenderTargetReadOnlyFlags & FBT_STENCIL) != 0)
							resetRenderPass = false;
						else
							resetRenderPass = true;
					}
					else
						resetRenderPass = true;
				}
				else
				{
					// Subresource has been bound as shader input, and it wasn't bound as a FB attachment this render pass.
					// However is could have been bound in a previous pass, so check the layouts and force a layout
					// transition if required.
					if (subresourceInfo.currentLayout != subresourceInfo.requiredLayout)
						resetRenderPass = true;
					else
						resetRenderPass = false;
				}
			}
		}

		// If we need to switch frame-buffers, end current render pass
		if (resetRenderPass && isInRenderPass())
			endRenderPass();
		else 
		{
			// Since we won't be ending the render pass, check if this same sub-resource was written earlier in the pass,
			// in which case we need to issue a memory barrier so those writes are visible.

			// Memory barrier only matters if image is bound for shader use (no need for color attachments or transfers)
			if(subresourceInfo.needsBarrier && isShaderBind)
			{
				bool isWrite = flags.isSet(VulkanUseFlag::Write);

				VkPipelineStageFlags stages =
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
					VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
					VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
					VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
					VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
					VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

				memoryBarrier(image->getHandle(), VK_ACCESS_SHADER_WRITE_BIT, 
							  image->getAccessFlags(subresourceInfo.requiredLayout, !isWrite),
							  stages, stages, subresourceInfo.requiredLayout, subresourceInfo.range);

				subresourceInfo.needsBarrier = isWrite;
			}
		}

		return resetRenderPass;
	}

	VulkanCmdBuffer::ImageSubresourceInfo& VulkanCmdBuffer::findSubresourceInfo(VulkanImage* image, UINT32 face, UINT32 mip)
	{
		UINT32 imageInfoIdx = mImages[image];
		ImageInfo& imageInfo = mImageInfos[imageInfoIdx];

		ImageSubresourceInfo* subresourceInfos = &mSubresourceInfoStorage[imageInfo.subresourceInfoIdx];
		for(UINT32 i = 0; i < imageInfo.numSubresourceInfos; i++)
		{
			ImageSubresourceInfo& entry = subresourceInfos[i];
			if(face >= entry.range.baseArrayLayer && face < (entry.range.baseArrayLayer + entry.range.layerCount) &&
			   mip >= entry.range.baseMipLevel && mip < (entry.range.baseMipLevel + entry.range.levelCount))
			{
				return entry;
			}
		}

		assert(false); // Caller should ensure the subresource actually exists, so this shouldn't happen
		return subresourceInfos[0];
	}

	void VulkanCmdBuffer::getInProgressQueries(Vector<VulkanTimerQuery*>& timer, Vector<VulkanOcclusionQuery*>& occlusion) const
	{
		for(auto& query : mTimerQueries)
		{
			if (query->_isInProgress())
				timer.push_back(query);
		}

		for (auto& query : mOcclusionQueries)
		{
			if (query->_isInProgress())
				occlusion.push_back(query);
		}
	}

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice& device, GpuQueueType type, UINT32 deviceIdx,
		UINT32 queueIdx, bool secondary)
		: CommandBuffer(type, deviceIdx, queueIdx, secondary), mBuffer(nullptr)
		, mDevice(device), mQueue(nullptr), mIdMask(0)
	{
		UINT32 numQueues = device.getNumQueues(mType);
		if (numQueues == 0) // Fall back to graphics queue
		{
			mType = GQT_GRAPHICS;
			numQueues = device.getNumQueues(GQT_GRAPHICS);
		}

		mQueue = device.getQueue(mType, mQueueIdx % numQueues);
		mIdMask = device.getQueueMask(mType, mQueueIdx);

		acquireNewBuffer();
	}

	RenderSurfaceMask VulkanCmdBuffer::getFBReadMask()
	{
		// Check if any frame-buffer attachments are also used as shader inputs, in which case we make them read-only
		RenderSurfaceMask readMask = RT_NONE;

		UINT32 numColorAttachments = mFramebuffer->getNumColorAttachments();
		for(UINT32 i = 0; i < numColorAttachments; i++)
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getColorAttachment(i);
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face,
				fbAttachment.surface.mipLevel);

			bool readOnly = subresourceInfo.isShaderInput;

			if(readOnly)
				readMask.set((RenderSurfaceMaskBits)(1 << i));
		}

		if (mFramebuffer->hasDepthAttachment())
		{
			const VulkanFramebufferAttachment& fbAttachment = mFramebuffer->getDepthStencilAttachment();
			ImageSubresourceInfo& subresourceInfo = findSubresourceInfo(fbAttachment.image, fbAttachment.surface.face,
				fbAttachment.surface.mipLevel);

			bool readOnly = subresourceInfo.isShaderInput;

			if (readOnly)
				readMask.set(RT_DEPTH);

			if ((mRenderTargetReadOnlyFlags & FBT_DEPTH) != 0)
				readMask.set(RT_DEPTH);

			if ((mRenderTargetReadOnlyFlags & FBT_STENCIL) != 0)
				readMask.set(RT_STENCIL);
		}

		return readMask;
	}


	VulkanCommandBuffer::~VulkanCommandBuffer()
	{
		mBuffer->reset();
	}

	void VulkanCommandBuffer::acquireNewBuffer()
	{
		VulkanCmdBufferPool& pool = mDevice.getCmdBufferPool();

		if (mBuffer != nullptr)
			assert(mBuffer->isSubmitted());

		UINT32 queueFamily = mDevice.getQueueFamily(mType);
		mBuffer = pool.getBuffer(queueFamily, mIsSecondary);
	}

	void VulkanCommandBuffer::submit(UINT32 syncMask)
	{
		// Ignore myself
		syncMask &= ~mIdMask;

		if (mBuffer->isInRenderPass())
			mBuffer->endRenderPass();

		// Execute any queued layout transitions that weren't already handled by the render pass
		mBuffer->executeLayoutTransitions();

		// Interrupt any in-progress queries (no in-progress queries allowed during command buffer submit)
		Vector<VulkanTimerQuery*> timerQueries;
		Vector<VulkanOcclusionQuery*> occlusionQueries;
		mBuffer->getInProgressQueries(timerQueries, occlusionQueries);

		for (auto& query : timerQueries)
			query->_interrupt(*mBuffer);

		for (auto& query : occlusionQueries)
			query->_interrupt(*mBuffer);

		if (mBuffer->isRecording())
			mBuffer->end();

		if (mBuffer->isReadyForSubmit()) // Possibly nothing was recorded in the buffer
		{
			mBuffer->submit(mQueue, mQueueIdx, syncMask);
			acquireNewBuffer();

			gVulkanCBManager().refreshStates(mDeviceIdx);
		}

		// Resume interrupted queries on the new command buffer
		for (auto& query : timerQueries)
			query->_resume(*mBuffer);

		for (auto& query : occlusionQueries)
			query->_resume(*mBuffer);
	}
}}

//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanHardwareBuffer.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"
#include "BsVulkanUtility.h"
#include "Managers/BsVulkanCommandBufferManager.h"
#include "BsVulkanCommandBuffer.h"
#include "BsVulkanTexture.h"

namespace bs { namespace ct
{
	VulkanBuffer::VulkanBuffer(VulkanResourceManager* owner, VkBuffer buffer, VmaAllocation allocation, UINT32 rowPitch
		, UINT32 slicePitch)
		: VulkanResource(owner, false), mBuffer(buffer), mAllocation(allocation), mRowPitch(rowPitch)
	{
		if (rowPitch != 0)
			mSliceHeight = slicePitch / rowPitch;
		else
			mSliceHeight = 0;
	}

	VulkanBuffer::~VulkanBuffer()
	{
		VulkanDevice& device = mOwner->getDevice();

		for(auto& entry : mViews)
			vkDestroyBufferView(device.getLogical(), entry.view, gVulkanAllocator);

		vkDestroyBuffer(device.getLogical(), mBuffer, gVulkanAllocator);
		device.freeMemory(mAllocation);
	}

	UINT8* VulkanBuffer::map(VkDeviceSize offset, VkDeviceSize length) const
	{
		VulkanDevice& device = mOwner->getDevice();

		VkDeviceMemory memory;
		VkDeviceSize memoryOffset;
		device.getAllocationInfo(mAllocation, memory, memoryOffset);

		UINT8* data;
		VkResult result = vkMapMemory(device.getLogical(), memory, memoryOffset + offset, length, 0, (void**)&data);
		assert(result == VK_SUCCESS);

		return data;
	}

	void VulkanBuffer::unmap()
	{
		VulkanDevice& device = mOwner->getDevice();

		VkDeviceMemory memory;
		VkDeviceSize memoryOffset;
		device.getAllocationInfo(mAllocation, memory, memoryOffset);

		vkUnmapMemory(device.getLogical(), memory);
	}

	void VulkanBuffer::copy(VulkanCmdBuffer* cb, VulkanBuffer* destination, VkDeviceSize srcOffset,
		VkDeviceSize dstOffset, VkDeviceSize length)
	{
		VkBufferCopy region;
		region.size = length;
		region.srcOffset = srcOffset;
		region.dstOffset = dstOffset;

		vkCmdCopyBuffer(cb->getHandle(), mBuffer, destination->getHandle(), 1, &region);
	}

	void VulkanBuffer::copy(VulkanCmdBuffer* cb, VulkanImage* destination, const VkExtent3D& extent,
		const VkImageSubresourceLayers& range, VkImageLayout layout)
	{
		VkBufferImageCopy region;
		region.bufferRowLength = mRowPitch;
		region.bufferImageHeight = mSliceHeight;
		region.bufferOffset = 0;
		region.imageOffset.x = 0;
		region.imageOffset.y = 0;
		region.imageOffset.z = 0;
		region.imageExtent = extent;
		region.imageSubresource = range;

		vkCmdCopyBufferToImage(cb->getHandle(), mBuffer, destination->getHandle(), layout, 1, &region);
	}

	void VulkanBuffer::update(VulkanCmdBuffer* cb, UINT8* data, VkDeviceSize offset, VkDeviceSize length)
	{
		vkCmdUpdateBuffer(cb->getHandle(), mBuffer, offset, length, (uint32_t*)data);
	}

	void VulkanBuffer::notifyDone(UINT32 globalQueueIdx, VulkanAccessFlags useFlags)
	{
		{
			Lock lock(mMutex);

			// Note: With often used buffers this block might never execute, in which case views won't  get freed.
			// If that ever becomes an issue (unlikely) then we'll need to track usage per-view.
			bool isLastHandle = mNumBoundHandles == 1;
			if (isLastHandle)
				destroyUnusedViews();
		}

		VulkanResource::notifyDone(globalQueueIdx, useFlags);
	}

	void VulkanBuffer::notifyUnbound()
	{
		{
			Lock lock(mMutex);

			// Note: With often used buffers this block might never execute, in which case views won't  get freed.
			// If that ever becomes an issue (unlikely) then we'll need to track usage per-view.
			bool isLastHandle = mNumBoundHandles == 1;
			if (isLastHandle)
				destroyUnusedViews();
		}

		VulkanResource::notifyUnbound();
	}

	VkBufferView VulkanBuffer::getView(VkFormat format)
	{
		const auto iterFind = std::find_if(mViews.begin(), mViews.end(),
			[format](const ViewInfo& x) { return x.format == format; });

		if(iterFind != mViews.end())
		{
			iterFind->useCount++;
			return iterFind->view;
		}

		VkBufferViewCreateInfo viewCI;
		viewCI.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
		viewCI.pNext = nullptr;
		viewCI.flags = 0;
		viewCI.offset = 0;
		viewCI.range = VK_WHOLE_SIZE;
		viewCI.format = format;
		viewCI.buffer = mBuffer;

		VkBufferView view;
		VkResult result = vkCreateBufferView(getDevice().getLogical(), &viewCI, gVulkanAllocator, &view);
		assert(result == VK_SUCCESS);

		mViews.push_back(ViewInfo(format, view));
		return view;
	}

	void VulkanBuffer::freeView(VkBufferView view)
	{
		const auto iterFind = std::find_if(mViews.begin(), mViews.end(),
			[view](const ViewInfo& x) { return x.view == view; });

		if(iterFind != mViews.end())
		{
			assert(iterFind->useCount > 0);
			iterFind->useCount--;
		}
		else
		{
			assert(false);
		}
	}
	
	void VulkanBuffer::destroyUnusedViews()
	{
		for(auto iter = mViews.begin(); iter != mViews.end();)
		{
			if(iter->useCount == 0)
			{
				vkDestroyBufferView(getDevice().getLogical(), iter->view, gVulkanAllocator);
				iter = mViews.erase(iter);
			}
			else
				++iter;
		}
	}

	VulkanHardwareBuffer::VulkanHardwareBuffer(BufferType type, GpuBufferFormat format, GpuBufferUsage usage,
		UINT32 size, GpuDeviceFlags deviceMask)
		: HardwareBuffer(size, usage, deviceMask), mBuffers(), mStagingBuffer(nullptr), mStagingMemory(nullptr)
		, mMappedDeviceIdx(-1), mMappedGlobalQueueIdx(-1), mMappedOffset(0), mMappedSize(0)
		, mMappedLockOptions(GBL_WRITE_ONLY), mDirectlyMappable((usage & GBU_DYNAMIC) != 0)
		, mSupportsGPUWrites(type == BT_STRUCTURED || ((usage & GBU_LOADSTORE) == GBU_LOADSTORE)), mIsMapped(false)
	{
		VkBufferUsageFlags usageFlags = 0;
		switch(type)
		{
		case BT_VERTEX:
			usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

			if((usage & GBU_LOADSTORE) == GBU_LOADSTORE)
				usageFlags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
			break;
		case BT_INDEX:
			usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

			if((usage & GBU_LOADSTORE) == GBU_LOADSTORE)
				usageFlags |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
			break;
		case BT_UNIFORM:
			usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			break;
		case BT_GENERIC:
			usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;

			if((usage & GBU_LOADSTORE) == GBU_LOADSTORE)
				usageFlags |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;

			break;
		case BT_STRUCTURED:
			usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			break;
		}

		mBufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		mBufferCI.pNext = nullptr;
		mBufferCI.flags = 0;
		mBufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		mBufferCI.usage = usageFlags;
		mBufferCI.queueFamilyIndexCount = 0;
		mBufferCI.pQueueFamilyIndices = nullptr;

		VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
		VulkanDevice* devices[BS_MAX_DEVICES];
		VulkanUtility::getDevices(rapi, deviceMask, devices);

		// Allocate buffers per-device
		for (UINT32 i = 0; i < BS_MAX_DEVICES; i++)
		{
			if (devices[i] == nullptr)
				continue;

			mBuffers[i] = createBuffer(*devices[i], size, false, true);
		}
	}

	VulkanHardwareBuffer::~VulkanHardwareBuffer()
	{
		for (UINT32 i = 0; i < BS_MAX_DEVICES; i++)
		{
			if (mBuffers[i] == nullptr)
				continue;

			mBuffers[i]->destroy();
		}

		assert(mStagingBuffer == nullptr);
	}

	VulkanBuffer* VulkanHardwareBuffer::createBuffer(VulkanDevice& device, UINT32 size, bool staging, bool readable)
	{
		VkBufferUsageFlags usage = mBufferCI.usage;
		if (staging)
		{
			mBufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

			// Staging buffers are used as a destination for reads
			if (readable)
				mBufferCI.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		}
		else if(readable) // Non-staging readable
			mBufferCI.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		mBufferCI.size = size;

		VkMemoryPropertyFlags flags;
		if(mDirectlyMappable || staging)
		{
			flags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;

#if BS_PLATFORM == BS_PLATFORM_OSX
			// Note: Use non-coherent memory when the buffer will be used as a uniform texel buffer. This is because
			// coherent memory gets allocated under 'shared' storage mode under Metal, which is not supported as backing
			// storage mode for textures (and a uniform texel buffer is classified as a texture in Metal). Technically
			// this still works but will cause a Metal validation error.
			//
			// Note that we also don't need to perform explicit flushing, despite being non-coherent, as that will be handled
			// by MoltenVK internally.
			if(staging || (usage & VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT) == 0)
				flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
#else
			flags |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
#endif
		}
		else
			flags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		VkDevice vkDevice = device.getLogical();

		VkBuffer buffer;
		VkResult result = vkCreateBuffer(vkDevice, &mBufferCI, gVulkanAllocator, &buffer);
		assert(result == VK_SUCCESS);

		VmaAllocation allocation = device.allocateMemory(buffer, flags);

		mBufferCI.usage = usage; // Restore original usage
		return device.getResourceManager().create<VulkanBuffer>(buffer, allocation);
	}

	void* VulkanHardwareBuffer::map(UINT32 offset, UINT32 length, GpuLockOptions options, UINT32 deviceIdx, UINT32 queueIdx)
	{
		if ((offset + length) > mSize)
		{
			BS_LOG(Error, RenderBackend, "Provided offset({0}) + length({1}) is larger than the buffer {2}.",
				offset, length, mSize);

			return nullptr;
		}

		if (length == 0)
			return nullptr;

		VulkanBuffer* buffer = mBuffers[deviceIdx];

		if (buffer == nullptr)
			return nullptr;

		mIsMapped = true;
		mMappedDeviceIdx = deviceIdx;
		mMappedGlobalQueueIdx = queueIdx;
		mMappedOffset = offset;
		mMappedSize = length;
		mMappedLockOptions = options;

		VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
		VulkanDevice& device = *rapi._getDevice(deviceIdx);

		VulkanCommandBufferManager& cbManager = gVulkanCBManager();
		GpuQueueType queueType;
		UINT32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(queueIdx, queueType);

		VkAccessFlags accessFlags;
		if (options == GBL_READ_ONLY)
			accessFlags = VK_ACCESS_HOST_READ_BIT;
		else if (options == GBL_READ_WRITE)
			accessFlags = VK_ACCESS_HOST_READ_BIT | VK_ACCESS_HOST_WRITE_BIT;
		else
			accessFlags = VK_ACCESS_HOST_WRITE_BIT;

		// If memory is host visible try mapping it directly
		if(mDirectlyMappable)
		{
			// Caller guarantees he won't touch the same data as the GPU, so just map even if the GPU is using the buffer
			if (options == GBL_WRITE_ONLY_NO_OVERWRITE)
				return buffer->map(offset, length);

			// Check is the GPU currently reading or writing from the buffer
			UINT32 useMask = buffer->getUseInfo(VulkanAccessFlag::Read | VulkanAccessFlag::Write);

			// Note: Even if GPU isn't currently using the buffer, but the buffer supports GPU writes, we consider it as
			// being used because the write could have completed yet still not visible, so we need to issue a pipeline
			// barrier below.
			bool isUsedOnGPU = useMask != 0 || mSupportsGPUWrites;

			// We're safe to map directly since GPU isn't using the buffer
			if (!isUsedOnGPU)
			{
				// If some CB has an operation queued that will be using the current contents of the buffer, create a new
				// buffer so we don't modify the previous use of the buffer
				if(buffer->isBound())
				{
					VulkanBuffer* newBuffer = createBuffer(device, mSize, false, true);

					// Copy contents of the current buffer to the new one, unless caller explicitly specifies he doesn't
					// care about the current contents
					if (options != GBL_WRITE_ONLY_DISCARD)
					{
						UINT8* src = buffer->map(offset, length);
						UINT8* dst = newBuffer->map(offset, length);

						memcpy(dst, src, length);

						buffer->unmap();
						newBuffer->unmap();
					}

					buffer->destroy();
					buffer = newBuffer;
					mBuffers[deviceIdx] = buffer;
				}

				return buffer->map(offset, length);
			}

			// Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
			if (options == GBL_WRITE_ONLY_DISCARD)
			{
				buffer->destroy();

				buffer = createBuffer(device, mSize, false, true);
				mBuffers[deviceIdx] = buffer;

				return buffer->map(offset, length);
			}

			// We need to read the buffer contents
			if(options == GBL_READ_ONLY || options == GBL_READ_WRITE)
			{
				// We need to wait until (potential) read/write operations complete
				VulkanTransferBuffer* transferCB = cbManager.getTransferBuffer(deviceIdx, queueType, localQueueIdx);

				// Ensure flush() will wait for all queues currently using to the buffer (if any) to finish
				// If only reading, wait for all writes to complete, otherwise wait on both writes and reads
				if (options == GBL_READ_ONLY)
					useMask = buffer->getUseInfo(VulkanAccessFlag::Write);
				else
					useMask = buffer->getUseInfo(VulkanAccessFlag::Read | VulkanAccessFlag::Write);

				transferCB->appendMask(useMask);

				// Make any writes visible before mapping
				if (mSupportsGPUWrites)
				{
					// Issue a barrier so :
					//  - If reading: the device makes the written memory available for read (read-after-write hazard)
					//  - If writing: ensures our writes properly overlap with GPU writes (write-after-write hazard)
					transferCB->memoryBarrier(buffer->getHandle(),
											  VK_ACCESS_SHADER_WRITE_BIT,
											  accessFlags,
											  // Last stages that could have written to the buffer:
											  VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
											  VK_PIPELINE_STAGE_HOST_BIT
					);
				}

				// Submit the command buffer and wait until it finishes
				transferCB->flush(true);

				// If writing and some CB has an operation queued that will be using the current contents of the buffer,
				// create a new  buffer so we don't modify the previous use of the buffer
				if (options == GBL_READ_WRITE && buffer->isBound())
				{
					VulkanBuffer* newBuffer = createBuffer(device, mSize, false, true);

					// Copy contents of the current buffer to the new one
					UINT8* src = buffer->map(offset, length);
					UINT8* dst = newBuffer->map(offset, length);

					memcpy(dst, src, length);

					buffer->unmap();
					newBuffer->unmap();

					buffer->destroy();
					buffer = newBuffer;
					mBuffers[deviceIdx] = buffer;
				}

				return buffer->map(offset, length);
			}

			// Otherwise, we're doing write only, in which case it's best to use the staging buffer to avoid waiting
			// and blocking, so fall through
		}

		// Can't use direct mapping, so use a staging buffer or memory

		// We might need to copy the current contents of the buffer to the staging buffer. Even if the user doesn't plan on
		// reading, it is still required as we will eventually copy all of the contents back to the original buffer,
		// and we can't write potentially uninitialized data. The only exception is when the caller specifies the buffer
		// contents should be discarded in which he guarantees he will overwrite the entire locked area with his own
		// contents.
		bool needRead = options != GBL_WRITE_ONLY_DISCARD_RANGE && options != GBL_WRITE_ONLY_DISCARD;
		
		// See if we can use the cheaper staging memory, rather than a staging buffer
		if(!needRead && offset % 4 == 0 && length % 4 == 0 && length <= 65536)
		{
			mStagingMemory = (UINT8*)bs_alloc(length);
			return mStagingMemory;
		}

		// Create a staging buffer
		mStagingBuffer = createBuffer(device, length, true, needRead);

		if (needRead)
		{
			VulkanTransferBuffer* transferCB = cbManager.getTransferBuffer(deviceIdx, queueType, localQueueIdx);
				
			// Similar to above, if buffer supports GPU writes or is currently being written to, we need to wait on any
			// potential writes to complete
			UINT32 writeUseMask = buffer->getUseInfo(VulkanAccessFlag::Write);
			if(mSupportsGPUWrites || writeUseMask != 0)
			{
				// Ensure flush() will wait for all queues currently writing to the buffer (if any) to finish
				transferCB->appendMask(writeUseMask);
			}

			// Queue copy command
			buffer->copy(transferCB->getCB(), mStagingBuffer, offset, 0, length);

			// Ensure data written to the staging buffer is visible
			transferCB->memoryBarrier(mStagingBuffer->getHandle(),
										VK_ACCESS_TRANSFER_WRITE_BIT,
										accessFlags,
										VK_PIPELINE_STAGE_TRANSFER_BIT,
										VK_PIPELINE_STAGE_HOST_BIT
			);

			// Submit the command buffer and wait until it finishes
			transferCB->flush(true);
			assert(!buffer->isUsed());
		}

		return mStagingBuffer->map(0, length);
	}

	void VulkanHardwareBuffer::unmap()
	{
		// Possibly map() failed with some error
		if (!mIsMapped)
			return;

		// Note: If we did any writes they need to be made visible to the GPU. However there is no need to execute
		// a pipeline barrier because (as per spec) host writes are implicitly visible to the device.

		if(mStagingMemory == nullptr && mStagingBuffer == nullptr) // We directly mapped the buffer
		{
			mBuffers[mMappedDeviceIdx]->unmap();
		}
		else
		{
			if(mStagingBuffer != nullptr)
				mStagingBuffer->unmap();

			bool isWrite = mMappedLockOptions != GBL_READ_ONLY;

			// We the caller wrote anything to the staging buffer, we need to upload it back to the main buffer
			if(isWrite)
			{
				VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
				VulkanDevice& device = *rapi._getDevice(mMappedDeviceIdx);

				VulkanCommandBufferManager& cbManager = gVulkanCBManager();
				GpuQueueType queueType;
				UINT32 localQueueIdx = CommandSyncMask::getQueueIdxAndType(mMappedGlobalQueueIdx, queueType);

				VulkanBuffer* buffer = mBuffers[mMappedDeviceIdx];
				VulkanTransferBuffer* transferCB = cbManager.getTransferBuffer(mMappedDeviceIdx, queueType, localQueueIdx);

				// If the buffer is used in any way on the GPU, we need to wait for that use to finish before
				// we issue our copy
				UINT32 useMask = buffer->getUseInfo(VulkanAccessFlag::Read | VulkanAccessFlag::Write);
				bool isNormalWrite = false;
				if(useMask != 0) // Buffer is currently used on the GPU
				{
					// Try to avoid the wait by checking for special write conditions

					// Caller guarantees he won't touch the same data as the GPU, so just copy
					if (mMappedLockOptions == GBL_WRITE_ONLY_NO_OVERWRITE)
					{
						// Fall through to copy()
					}
					// Caller doesn't care about buffer contents, so just discard the existing buffer and create a new one
					else if (mMappedLockOptions == GBL_WRITE_ONLY_DISCARD)
					{
						buffer->destroy();

						buffer = createBuffer(device, mSize, false, true);
						mBuffers[mMappedDeviceIdx] = buffer;
					}
					else // Otherwise we have no choice but to issue a dependency between the queues
					{
						transferCB->appendMask(useMask);
						isNormalWrite = true;
					}
				}
				else
					isNormalWrite = true;

				// Check if the buffer will still be bound somewhere after the CBs using it finish
				if (isNormalWrite)
				{
					UINT32 useCount = buffer->getUseCount();
					UINT32 boundCount = buffer->getBoundCount();

					bool isBoundWithoutUse = boundCount > useCount;

					// If buffer is queued for some operation on a CB, then we need to make a copy of the buffer to
					// avoid modifying its use in the previous operation
					if (isBoundWithoutUse)
					{
						VulkanBuffer* newBuffer = createBuffer(device, mSize, false, true);

						// Avoid copying original contents if the staging buffer completely covers it
						if (mMappedOffset > 0 || mMappedSize != mSize)
						{
							buffer->copy(transferCB->getCB(), newBuffer, 0, 0, mSize);

							transferCB->getCB()->registerBuffer(buffer, BufferUseFlagBits::Transfer, VulkanAccessFlag::Read);
						}

						buffer->destroy();
						buffer = newBuffer;
						mBuffers[mMappedDeviceIdx] = buffer;
					}
				}

				// Queue copy/update command
				if (mStagingBuffer != nullptr)
				{
					mStagingBuffer->copy(transferCB->getCB(), buffer, 0, mMappedOffset, mMappedSize);
					transferCB->getCB()->registerBuffer(mStagingBuffer, BufferUseFlagBits::Transfer, VulkanAccessFlag::Read);
				}
				else // Staging memory
				{
					buffer->update(transferCB->getCB(), mStagingMemory, mMappedOffset, mMappedSize);
				}

				transferCB->getCB()->registerBuffer(buffer, BufferUseFlagBits::Transfer, VulkanAccessFlag::Write);

				// We don't actually flush the transfer buffer here since it's an expensive operation, but it's instead
				// done automatically before next "normal" command buffer submission.
			}

			if (mStagingBuffer != nullptr)
			{
				mStagingBuffer->destroy();
				mStagingBuffer = nullptr;
			}

			if(mStagingMemory != nullptr)
			{
				bs_free(mStagingMemory);
				mStagingMemory = nullptr;
			}
		}

		mIsMapped = false;
	}

	void VulkanHardwareBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset,
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer, const SPtr<CommandBuffer>& commandBuffer)
	{
		if ((dstOffset + length) > mSize)
		{
			BS_LOG(Error, RenderBackend, "Provided offset({0}) + length({1}) is larger than the destination buffer {2}. "
				"Copy operation aborted.", dstOffset, length, mSize);

			return;
		}

		if ((srcOffset + length) > srcBuffer.getSize())
		{
			BS_LOG(Error, RenderBackend, "Provided offset({0}) + length({1}) is larger than the source buffer {2}. "
				"Copy operation aborted.", srcOffset, length, srcBuffer.getSize());

			return;
		}

		VulkanHardwareBuffer& vkSource = static_cast<VulkanHardwareBuffer&>(srcBuffer);

		VulkanRenderAPI& rapi = static_cast<VulkanRenderAPI&>(RenderAPI::instance());
		VulkanCmdBuffer* vkCB;
		if (commandBuffer != nullptr)
			vkCB = static_cast<VulkanCommandBuffer*>(commandBuffer.get())->getInternal();
		else
			vkCB = rapi._getMainCommandBuffer()->getInternal();

		UINT32 deviceIdx = vkCB->getDeviceIdx();

		VulkanBuffer* src = vkSource.mBuffers[deviceIdx];
		VulkanBuffer* dst = mBuffers[deviceIdx];

		if (src == nullptr || dst == nullptr)
			return;

		if (vkCB->isInRenderPass())
			vkCB->endRenderPass();

		src->copy(vkCB, dst, srcOffset, dstOffset, length);

		// Notify the command buffer that these resources are being used on it
		vkCB->registerBuffer(src, BufferUseFlagBits::Transfer, VulkanAccessFlag::Read);
		vkCB->registerBuffer(dst, BufferUseFlagBits::Transfer, VulkanAccessFlag::Write);
	}

	void VulkanHardwareBuffer::readData(UINT32 offset, UINT32 length, void* dest, UINT32 deviceIdx, UINT32 queueIdx)
	{
		void* lockedData = lock(offset, length, GBL_READ_ONLY, deviceIdx, queueIdx);
		memcpy(dest, lockedData, length);
		unlock();
	}

	void VulkanHardwareBuffer::writeData(UINT32 offset, UINT32 length, const void* source, BufferWriteType writeFlags,
		UINT32 queueIdx)
	{
		GpuLockOptions lockOptions = GBL_WRITE_ONLY_DISCARD_RANGE;

		if (writeFlags == BTW_NO_OVERWRITE)
			lockOptions = GBL_WRITE_ONLY_NO_OVERWRITE;
		else if (writeFlags == BWT_DISCARD)
			lockOptions = GBL_WRITE_ONLY_DISCARD;

		// Write to every device
		for (UINT32 i = 0; i < BS_MAX_DEVICES; i++)
		{
			if (mBuffers[i] == nullptr)
				continue;

			void* lockedData = lock(offset, length, lockOptions, i, queueIdx);
			memcpy(lockedData, source, length);
			unlock();
		}
	}
}}

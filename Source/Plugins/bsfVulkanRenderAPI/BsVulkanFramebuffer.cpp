//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanFramebuffer.h"
#include "BsVulkanTexture.h"
#include "BsVulkanUtility.h"
#include "BsVulkanDevice.h"

namespace bs { namespace ct
{
	VulkanFramebuffer::VariantKey::VariantKey(RenderSurfaceMask loadMask, RenderSurfaceMask readMask, 
		ClearMask clearMask)
		:loadMask(loadMask), readMask(readMask), clearMask(clearMask)
	{ }

	size_t VulkanFramebuffer::VariantKey::HashFunction::operator()(const VariantKey& v) const
	{
		size_t hash = 0;
		hash_combine(hash, v.readMask);
		hash_combine(hash, v.loadMask);
		hash_combine(hash, v.clearMask);

		return hash;
	}

	bool VulkanFramebuffer::VariantKey::EqualFunction::operator()(const VariantKey& lhs,
																  const VariantKey& rhs) const
	{
		return lhs.loadMask == rhs.loadMask && lhs.readMask == rhs.readMask && lhs.clearMask == rhs.clearMask;
	}

	UINT32 VulkanFramebuffer::sNextValidId = 1;

	VulkanFramebuffer::VulkanFramebuffer(VulkanResourceManager* owner, const VULKAN_FRAMEBUFFER_DESC& desc)
		: VulkanResource(owner, false), mNumAttachments(0), mNumColorAttachments(0), mNumLayers(desc.layers)
		, mColorAttachments(), mDepthStencilAttachment(), mHasDepth(false), mSampleFlags(VK_SAMPLE_COUNT_1_BIT)
	{
		mId = sNextValidId++;

		mSampleFlags = VulkanUtility::getSampleFlags(desc.numSamples);

		UINT32 attachmentIdx = 0;
		for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (desc.color[i].image == nullptr)
				continue;

			VkAttachmentDescription& attachmentDesc = mAttachments[attachmentIdx];
			attachmentDesc.flags = 0;
			attachmentDesc.format = desc.color[i].format;
			attachmentDesc.samples = mSampleFlags;
			attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			// If offscreen we make the assumption the surface will be read by a shader
			if(desc.offscreen)
				attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			else
				attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			mColorAttachments[attachmentIdx].baseLayer = desc.color[i].baseLayer;
			mColorAttachments[attachmentIdx].image = desc.color[i].image;
			mColorAttachments[attachmentIdx].finalLayout = attachmentDesc.finalLayout;
			mColorAttachments[attachmentIdx].index = i;
			mColorAttachments[attachmentIdx].surface = desc.color[i].surface;

			VkAttachmentReference& ref = mColorReferences[attachmentIdx];
			ref.attachment = attachmentIdx;
			ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			if (desc.color[i].surface.numMipLevels == 0)
				mAttachmentViews[attachmentIdx] = desc.color[i].image->getView(true);
			else
				mAttachmentViews[attachmentIdx] = desc.color[i].image->getView(desc.color[i].surface, true);

			attachmentIdx++;
		}

		mNumColorAttachments = attachmentIdx;
		mHasDepth = desc.depth.image != nullptr;

		if (mHasDepth)
		{
			VkAttachmentDescription& attachmentDesc = mAttachments[attachmentIdx];
			attachmentDesc.flags = 0;
			attachmentDesc.format = desc.depth.format;
			attachmentDesc.samples = mSampleFlags;
			attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			attachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
			attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			attachmentDesc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			mDepthStencilAttachment.baseLayer = desc.depth.baseLayer;
			mDepthStencilAttachment.image = desc.depth.image;
			mDepthStencilAttachment.finalLayout = attachmentDesc.finalLayout;
			mDepthStencilAttachment.index = 0;
			mDepthStencilAttachment.surface = desc.depth.surface;

			VkAttachmentReference& ref = mDepthReference;
			ref.attachment = attachmentIdx;
			ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			if (desc.depth.surface.numMipLevels == 0)
				mAttachmentViews[attachmentIdx] = desc.depth.image->getView(true);
			else
				mAttachmentViews[attachmentIdx] = desc.depth.image->getView(desc.depth.surface, true);

			attachmentIdx++;
		}

		mNumAttachments = attachmentIdx;

		mSubpassDesc.flags = 0;
		mSubpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		mSubpassDesc.colorAttachmentCount = mNumColorAttachments;
		mSubpassDesc.inputAttachmentCount = 0;
		mSubpassDesc.pInputAttachments = nullptr;
		mSubpassDesc.preserveAttachmentCount = 0;
		mSubpassDesc.pPreserveAttachments = nullptr;
		mSubpassDesc.pResolveAttachments = nullptr;

		if (mNumColorAttachments > 0)
			mSubpassDesc.pColorAttachments = mColorReferences;
		else
			mSubpassDesc.pColorAttachments = nullptr;

		if (mHasDepth)
			mSubpassDesc.pDepthStencilAttachment = &mDepthReference;
		else
			mSubpassDesc.pDepthStencilAttachment = nullptr;

		// Subpass dependencies for layout transitions
		mDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		mDependencies[0].dstSubpass = 0;
		mDependencies[0].srcStageMask = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		
		#if BS_PLATFORM != BS_PLATFORM_OSX
		mDependencies[0].dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		#else
		// see: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/man/html/VkAccessFlagBits.html
		mDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
			VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
			/*VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
			VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV |*/
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT /*|
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT*/;
		#endif
		
		mDependencies[0].srcAccessMask = 0;
		mDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | 
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		mDependencies[0].dependencyFlags = 0;

		mDependencies[1].srcSubpass = 0;
		mDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		
		#if BS_PLATFORM != BS_PLATFORM_OSX
		mDependencies[1].srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		#else
		mDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
		/*VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV |
		VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV |*/
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT /*|
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT*/;
		#endif
		
		mDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		mDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | 
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		mDependencies[1].dstAccessMask = 0;
		mDependencies[1].dependencyFlags = 0;

		// Create render pass and frame buffer create infos
		mRenderPassCI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		mRenderPassCI.pNext = nullptr;
		mRenderPassCI.flags = 0;
		mRenderPassCI.attachmentCount = mNumAttachments;
		mRenderPassCI.pAttachments = mAttachments;
		mRenderPassCI.subpassCount = 1;
		mRenderPassCI.pSubpasses = &mSubpassDesc;
		mRenderPassCI.dependencyCount = 2;
		mRenderPassCI.pDependencies = mDependencies;

		mFramebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		mFramebufferCI.pNext = nullptr;
		mFramebufferCI.flags = 0;
		mFramebufferCI.renderPass = VK_NULL_HANDLE;
		mFramebufferCI.attachmentCount = mNumAttachments;
		mFramebufferCI.pAttachments = mAttachmentViews;
		mFramebufferCI.width = desc.width;
		mFramebufferCI.height = desc.height;
		mFramebufferCI.layers = desc.layers;

		mDefault = createVariant(RT_NONE, RT_NONE, CLEAR_NONE);		
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = mOwner->getDevice().getLogical();

		vkDestroyFramebuffer(device, mDefault.framebuffer, gVulkanAllocator);
		vkDestroyRenderPass(device, mDefault.renderPass, gVulkanAllocator);

		for(auto& entry : mVariants)
		{
			vkDestroyFramebuffer(device, entry.second.framebuffer, gVulkanAllocator);
			vkDestroyRenderPass(device, entry.second.renderPass, gVulkanAllocator);
		}
	}

	VulkanFramebuffer::Variant VulkanFramebuffer::createVariant(RenderSurfaceMask loadMask, 
		RenderSurfaceMask readMask, ClearMask clearMask) const
	{
		for (UINT32 i = 0; i < mNumColorAttachments; i++)
		{
			const VulkanFramebufferAttachment& attachment = mColorAttachments[i];
			VkAttachmentDescription& attachmentDesc = mAttachments[i];
			VkAttachmentReference& attachmentRef = mColorReferences[i];

			if (loadMask.isSet((RenderSurfaceMaskBits)(1 << attachment.index)))
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if (clearMask.isSet((ClearMaskBits)(1 << attachment.index)))
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			if(readMask.isSet((RenderSurfaceMaskBits)(1 << attachment.index)))
				attachmentRef.layout = VK_IMAGE_LAYOUT_GENERAL;
			else
				attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}

		if (mHasDepth)
		{
			VkAttachmentDescription& attachmentDesc = mAttachments[mNumColorAttachments];
			VkAttachmentReference& attachmentRef = mDepthReference;

			if (loadMask.isSet(RT_DEPTH) || loadMask.isSet(RT_STENCIL))
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
			else
			{
				if(clearMask.isSet(CLEAR_DEPTH))
					attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				else
					attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

				if(clearMask.isSet(CLEAR_STENCIL))
					attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				else
					attachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;

				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			// When depth-stencil is readable it's up to the caller to ensure he doesn't try to write to it as well, so we
			// just assume a read-only layout.
			if (readMask.isSet(RT_DEPTH))
			{
				if (readMask.isSet(RT_STENCIL))
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
				else // Depth readable but stencil isn't
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL_KHR;
			}
			else
			{
				if (readMask.isSet(RT_STENCIL)) // Stencil readable but depth isn't
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL_KHR;
				else 
					attachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
			}
		}

		VkDevice device = mOwner->getDevice().getLogical();

		Variant variant;
		VkResult result = vkCreateRenderPass(device, &mRenderPassCI, gVulkanAllocator, &variant.renderPass);
		assert(result == VK_SUCCESS);

		mFramebufferCI.renderPass = variant.renderPass;

		result = vkCreateFramebuffer(device, &mFramebufferCI, gVulkanAllocator, &variant.framebuffer);
		assert(result == VK_SUCCESS);

		return variant;
	}

	VkRenderPass VulkanFramebuffer::getRenderPass(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
												  ClearMask clearMask) const
	{
		if (loadMask == RT_NONE && readMask == RT_NONE && clearMask == CLEAR_NONE)
			return mDefault.renderPass;

		VariantKey key(loadMask, readMask, clearMask);
		auto iterFind = mVariants.find(key);
		if (iterFind != mVariants.end())
			return iterFind->second.renderPass;

		Variant newVariant = createVariant(loadMask, readMask, clearMask);
		mVariants[key] = newVariant;

		return newVariant.renderPass;
	}

	VkFramebuffer VulkanFramebuffer::getFramebuffer(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
													ClearMask clearMask) const
	{
		if (loadMask == RT_NONE && readMask == RT_NONE && clearMask == CLEAR_NONE)
			return mDefault.framebuffer;

		VariantKey key(loadMask, readMask, clearMask);
		auto iterFind = mVariants.find(key);
		if (iterFind != mVariants.end())
			return iterFind->second.framebuffer;

		Variant newVariant = createVariant(loadMask, readMask, clearMask);
		mVariants[key] = newVariant;

		return newVariant.framebuffer;
	}

	UINT32 VulkanFramebuffer::getNumClearEntries(ClearMask clearMask) const
	{
		if (clearMask == CLEAR_NONE)
			return 0;
		else if (clearMask == CLEAR_ALL)
			return getNumAttachments();
		else if (((UINT32)clearMask & (UINT32)(CLEAR_DEPTH | CLEAR_STENCIL)) != 0 && hasDepthAttachment())
			return getNumAttachments();

		UINT32 numAttachments = 0;
		for(INT32 i = BS_MAX_MULTIPLE_RENDER_TARGETS - 1; i >= 0; i--)
		{
			if(((1 << i) & (UINT32)clearMask) != 0)
			{
				numAttachments = i + 1;
				break;
			}
		}

		return std::min(numAttachments, getNumColorAttachments());
	}
}}

//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanRenderPass.h"
#include "BsVulkanTexture.h"
#include "BsVulkanUtility.h"
#include "BsVulkanDevice.h"

namespace bs { namespace ct
{
	VulkanRenderPass::VariantKey::VariantKey(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
		ClearMask clearMask)
		:loadMask(loadMask), readMask(readMask), clearMask(clearMask)
	{ }

	size_t VulkanRenderPass::VariantKey::HashFunction::operator()(const VariantKey& v) const
	{
		size_t hash = 0;
		bs_hash_combine(hash, v.readMask);
		bs_hash_combine(hash, v.loadMask);
		bs_hash_combine(hash, v.clearMask);

		return hash;
	}

	bool VulkanRenderPass::VariantKey::EqualFunction::operator()(
		const VariantKey& lhs, const VariantKey& rhs) const
	{
		return lhs.loadMask == rhs.loadMask && lhs.readMask == rhs.readMask && lhs.clearMask == rhs.clearMask;
	}

	UINT32 VulkanRenderPass::sNextValidId = 1;

	VulkanRenderPass::VulkanRenderPass(const VkDevice& device, const VULKAN_RENDER_PASS_DESC& desc)
		:mDevice(device)
	{
		mId = sNextValidId++;
		mSampleFlags = VulkanUtility::getSampleFlags(desc.numSamples);

		UINT32 attachmentIdx = 0;
		for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if(!desc.color[i].enabled)
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

			VkAttachmentReference& ref = mColorReferences[attachmentIdx];
			ref.attachment = attachmentIdx;
			ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			mIndices[attachmentIdx] = i;
			attachmentIdx++;
		}

		mNumColorAttachments = attachmentIdx;
		mHasDepth = desc.depth.enabled;

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

			VkAttachmentReference& ref = mDepthReference;
			ref.attachment = attachmentIdx;
			ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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
		mDependencies[0].dstStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		mDependencies[0].srcAccessMask = 0;
		mDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;
		mDependencies[0].dependencyFlags = 0;

		mDependencies[1].srcSubpass = 0;
		mDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		mDependencies[1].srcStageMask = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
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

		mDefault = createVariant(RT_NONE, RT_NONE, CLEAR_NONE);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		for(auto& entry : mVariants)
			vkDestroyRenderPass(mDevice, entry.second, gVulkanAllocator);
	}

	VkRenderPass VulkanRenderPass::createVariant(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
		ClearMask clearMask) const
	{
		for (UINT32 i = 0; i < mNumColorAttachments; i++)
		{
			VkAttachmentDescription& attachmentDesc = mAttachments[i];
			VkAttachmentReference& attachmentRef = mColorReferences[i];
			UINT32 index = mIndices[i];

			if (loadMask.isSet((RenderSurfaceMaskBits)(1 << index)))
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			}
			else if (clearMask.isSet((ClearMaskBits)(1 << index)))
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			else
			{
				attachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				attachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}

			if(readMask.isSet((RenderSurfaceMaskBits)(1 << index)))
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

		VkRenderPass output;
		VkResult result = vkCreateRenderPass(mDevice, &mRenderPassCI, gVulkanAllocator, &output);
		assert(result == VK_SUCCESS);

		return output;
	}

	VkRenderPass VulkanRenderPass::getVkRenderPass(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
		ClearMask clearMask) const
	{
		if (loadMask == RT_NONE && readMask == RT_NONE && clearMask == CLEAR_NONE)
			return mDefault;

		VariantKey key(loadMask, readMask, clearMask);
		auto iterFind = mVariants.find(key);
		if (iterFind != mVariants.end())
			return iterFind->second;

		VkRenderPass newVariant = createVariant(loadMask, readMask, clearMask);
		mVariants[key] = newVariant;

		return newVariant;
	}

	UINT32 VulkanRenderPass::getNumClearEntries(ClearMask clearMask) const
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


	VulkanRenderPasses::~VulkanRenderPasses()
	{
		for(auto& entry : mVariants)
			bs_delete(entry.second);
	}

	VulkanRenderPass* VulkanRenderPasses::get(const VkDevice& device, const VULKAN_RENDER_PASS_DESC& desc)
	{
		VariantKey key(device, desc);

		VulkanRenderPass* pass;
		{
			Lock lock(mMutex);

			auto iterFind = mVariants.find(key);
			if (iterFind != mVariants.end())
				return iterFind->second;

			pass = bs_new<VulkanRenderPass>(device, desc);
			mVariants[key] = pass;
		}

		return pass;
	}

	VulkanRenderPasses::VariantKey::VariantKey(const VkDevice& device, const VULKAN_RENDER_PASS_DESC& desc)
		:device(device), desc(desc)
	{ }

	size_t VulkanRenderPasses::VariantKey::HashFunction::operator()(const VariantKey& v) const
	{
		size_t hash = 0;
		bs_hash_combine(hash, v.device);
		bs_hash_combine(hash, v.desc.offscreen);
		bs_hash_combine(hash, v.desc.numSamples);
		bs_hash_combine(hash, v.desc.depth.enabled);
		bs_hash_combine(hash, v.desc.depth.format);

		for(UINT32 i = 0; i < bs_size(v.desc.color); i++)
		{
			bs_hash_combine(hash, v.desc.color[i].enabled);
			bs_hash_combine(hash, v.desc.color[i].format);
		}

		return hash;
	}

	bool VulkanRenderPasses::VariantKey::EqualFunction::operator()(const VariantKey& lhs, const VariantKey& rhs) const
	{
		if(lhs.device != rhs.device ||
			lhs.desc.offscreen != rhs.desc.offscreen ||
			lhs.desc.numSamples != rhs.desc.numSamples ||
			lhs.desc.depth.enabled != rhs.desc.depth.enabled ||
			lhs.desc.depth.format != rhs.desc.depth.format)
			return false;

		for(UINT32 i = 0; i < bs_size(lhs.desc.color); i++)
		{
			if(lhs.desc.color[i].enabled != rhs.desc.color[i].enabled ||
				lhs.desc.color[i].format != rhs.desc.color[i].format)
				return false;
		}

		return true;
	}

}}

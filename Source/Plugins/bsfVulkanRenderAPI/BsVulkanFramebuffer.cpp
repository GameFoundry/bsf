//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsVulkanFramebuffer.h"
#include "BsVulkanTexture.h"
#include "BsVulkanUtility.h"
#include "BsVulkanDevice.h"
#include "BsVulkanRenderPass.h"

namespace bs { namespace ct
{
	VulkanFramebuffer::VariantKey::VariantKey(RenderSurfaceMask loadMask, RenderSurfaceMask readMask, 
		ClearMask clearMask)
		:loadMask(loadMask), readMask(readMask), clearMask(clearMask)
	{ }

	size_t VulkanFramebuffer::VariantKey::HashFunction::operator()(const VariantKey& v) const
	{
		size_t hash = 0;
		bs_hash_combine(hash, v.readMask);
		bs_hash_combine(hash, v.loadMask);
		bs_hash_combine(hash, v.clearMask);

		return hash;
	}

	bool VulkanFramebuffer::VariantKey::EqualFunction::operator()(
		const VariantKey& lhs, const VariantKey& rhs) const
	{
		return lhs.loadMask == rhs.loadMask && lhs.readMask == rhs.readMask && lhs.clearMask == rhs.clearMask;
	}

	UINT32 VulkanFramebuffer::sNextValidId = 1;

	VulkanFramebuffer::VulkanFramebuffer(VulkanResourceManager* owner, VulkanRenderPass* renderPass, 
		const VULKAN_FRAMEBUFFER_DESC& desc)
		: VulkanResource(owner, false), mRenderPass(renderPass), mNumLayers(desc.layers)
	{
		mId = sNextValidId++;

		UINT32 attachmentIdx = 0;
		for(UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			if (desc.color[i].image == nullptr)
				continue;

			mColorAttachments[attachmentIdx].baseLayer = desc.color[i].baseLayer;
			mColorAttachments[attachmentIdx].image = desc.color[i].image;
			mColorAttachments[attachmentIdx].finalLayout = renderPass->getColorDesc(attachmentIdx).finalLayout;
			mColorAttachments[attachmentIdx].index = i;
			mColorAttachments[attachmentIdx].surface = desc.color[i].surface;

			if (desc.color[i].surface.numMipLevels == 0)
				mAttachmentViews[attachmentIdx] = desc.color[i].image->getView(true);
			else
				mAttachmentViews[attachmentIdx] = desc.color[i].image->getView(desc.color[i].surface, true);

			attachmentIdx++;
		}

		if (renderPass->hasDepthAttachment())
		{
			mDepthStencilAttachment.baseLayer = desc.depth.baseLayer;
			mDepthStencilAttachment.image = desc.depth.image;
			mDepthStencilAttachment.finalLayout = renderPass->getDepthDesc().finalLayout;
			mDepthStencilAttachment.index = 0;
			mDepthStencilAttachment.surface = desc.depth.surface;

			if (desc.depth.surface.numMipLevels == 0)
				mAttachmentViews[attachmentIdx] = desc.depth.image->getView(true);
			else
				mAttachmentViews[attachmentIdx] = desc.depth.image->getView(desc.depth.surface, true);

			attachmentIdx++;
		}

		mFramebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		mFramebufferCI.pNext = nullptr;
		mFramebufferCI.flags = 0;
		mFramebufferCI.renderPass = VK_NULL_HANDLE;
		mFramebufferCI.attachmentCount = renderPass->getNumAttachments();
		mFramebufferCI.pAttachments = mAttachmentViews;
		mFramebufferCI.width = desc.width;
		mFramebufferCI.height = desc.height;
		mFramebufferCI.layers = desc.layers;

		mDefault = createVariant(RT_NONE, RT_NONE, CLEAR_NONE);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VkDevice device = mOwner->getDevice().getLogical();

		vkDestroyFramebuffer(device, mDefault, gVulkanAllocator);

		for(auto& entry : mVariants)
			vkDestroyFramebuffer(device, entry.second, gVulkanAllocator);
	}

	VkFramebuffer VulkanFramebuffer::createVariant(RenderSurfaceMask loadMask, 
		RenderSurfaceMask readMask, ClearMask clearMask) const
	{
		VkDevice device = mOwner->getDevice().getLogical();

		mFramebufferCI.renderPass = mRenderPass->getVkRenderPass(loadMask, readMask, clearMask);

		VkFramebuffer output;
		VkResult result = vkCreateFramebuffer(device, &mFramebufferCI, gVulkanAllocator, &output);
		assert(result == VK_SUCCESS);

		return output;
	}

	VkFramebuffer VulkanFramebuffer::getVkFramebuffer(RenderSurfaceMask loadMask, RenderSurfaceMask readMask,
		ClearMask clearMask) const
	{
		if (loadMask == RT_NONE && readMask == RT_NONE && clearMask == CLEAR_NONE)
			return mDefault;

		VariantKey key(loadMask, readMask, clearMask);
		auto iterFind = mVariants.find(key);
		if (iterFind != mVariants.end())
			return iterFind->second;

		VkFramebuffer newVariant = createVariant(loadMask, readMask, clearMask);
		mVariants[key] = newVariant;

		return newVariant;
	}
}}
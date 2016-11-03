//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanTexture.h"
#include "BsVulkanDevice.h"
#include "BsCoreThread.h"
#include "BsException.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	VulkanImage::VulkanImage(VulkanResourceManager* owner, VkImage image, VkImageLayout layout)
		:VulkanResource(owner, false, VulkanResourceType::Image), mImage(image), mLayout(layout)
	{
		
	}

	VulkanImage::~VulkanImage()
	{
		vkDestroyImage(mOwner->getDevice().getLogical(), mImage, gVulkanAllocator);
	}

	VulkanTextureCore::VulkanTextureCore(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData,
		GpuDeviceFlags deviceMask)
		: TextureCore(desc, initialData, deviceMask), mImages()
	{
		
	}

	VulkanTextureCore::~VulkanTextureCore()
	{ 
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Texture);
	}

	void VulkanTextureCore::initialize()
	{
		THROW_IF_NOT_CORE_THREAD;

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Texture);
		TextureCore::initialize();
	}

	VkImageView VulkanTextureCore::getView(UINT32 deviceIdx)
	{
		// TODO
		// - If device idx doesn't match the mask, return VK_NULL_HANDLE
		// - Otherwise return the default image view (created by default in initialize())
		// - Free the view in destructor
	}

	VkImageView VulkanTextureCore::getView(UINT32 deviceIdx, const TextureSurface& surface)
	{
		// TODO
		// - If device idx doesn't match the mask, return VK_NULL_HANDLE
		// - Otherwise
		//  - Scan the mTextureViews array for an existing view matching the surface
		//  - If view isn't found create a new one and add it to mTextureViews array
		//   - Resize the mTextureViews array as needed
		//   - By default mTextureViews is nullptr, so allocate it during first call
		// - Free the views in destructor (if any were allocated)
	}

	void VulkanTextureCore::copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target)
	{
		
	}

	PixelData VulkanTextureCore::lockImpl(GpuLockOptions options, UINT32 mipLevel, UINT32 face)
	{
		PixelData lockedArea(1, 1, 1, mProperties.getFormat());
	
		// TODO - Increment read/write stat (do this for other buffers as well)

		return lockedArea;
	}

	void VulkanTextureCore::unlockImpl()
	{
		
	}

	void VulkanTextureCore::readData(PixelData& dest, UINT32 mipLevel, UINT32 face)
	{
		BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_GpuBuffer);
	}

	void VulkanTextureCore::writeData(const PixelData& src, UINT32 mipLevel, UINT32 face, bool discardWholeBuffer)
	{
		BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_Texture);
	}
}
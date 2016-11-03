//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Wrapper around a Vulkan image object that manages its usage and lifetime. */
	class VulkanImage : public VulkanResource
	{
	public:
		VulkanImage(VulkanResourceManager* owner, VkImage image, VkImageLayout layout);
		~VulkanImage();

		/** Returns the internal handle to the Vulkan object. */
		VkImage getHandle() const { return mImage; }

		/** Returns the layout the image is currently in. */
		VkImageLayout getLayout() const { return mLayout; }

		/** Notifies the resource that the current image layout has changed. */
		void setLayout(VkImageLayout layout) { mLayout = layout; }

	private:
		VkImage mImage;
		VkImageLayout mLayout;
	};

	/**	Vulkan implementation of a texture. */
	class VulkanTextureCore : public TextureCore
	{
	public:
		~VulkanTextureCore();

		/** 
		 * Gets the resource wrapping the Vulkan image object, on the specified device. If texture device mask doesn't 
		 * include the provided device, null is returned. 
		 */
		VulkanImage* getResource(UINT32 deviceIdx) const { return mImages[deviceIdx]; }

		/** 
		 * Returns an image view that covers all faces and mip maps of the texture. Usable only on the specified device. 
		 * If texture device mask doesn't include the provided device, null is returned. 
		 */
		VkImageView getView(UINT32 deviceIdx);

		/** 
		 * Returns an image view that covers the specified faces and mip maps of the texture. Usable only on the specified 
		 * device. If texture device mask doesn't include the provided device, null is returned. 
		 */
		VkImageView getView(UINT32 deviceIdx, const TextureSurface& surface);

	protected:
		friend class VulkanTextureCoreManager;

		VulkanTextureCore(const TEXTURE_DESC& desc, const SPtr<PixelData>& initialData, GpuDeviceFlags deviceMask);

		/** @copydoc CoreObjectCore::initialize() */
		void initialize() override;

		/** @copydoc TextureCore::lockImpl */
		PixelData lockImpl(GpuLockOptions options, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/** @copydoc TextureCore::unlockImpl */
		void unlockImpl() override;

		/** @copydoc TextureCore::copyImpl */
		void copyImpl(UINT32 srcFace, UINT32 srcMipLevel, UINT32 destFace, UINT32 destMipLevel, const SPtr<TextureCore>& target) override;

		/** @copydoc TextureCore::readData */
		void readData(PixelData& dest, UINT32 mipLevel = 0, UINT32 face = 0) override;

		/** @copydoc TextureCore::writeData */
		void writeData(const PixelData& src, UINT32 mipLevel = 0, UINT32 face = 0, bool discardWholeBuffer = false) override;

	private:
		VulkanImage* mImages[BS_MAX_DEVICES];
	};

	/** @} */
}
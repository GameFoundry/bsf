//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "Managers/BsTextureManager.h"

namespace bs
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/**	Handles creation of Vulkan textures. */
	class VulkanTextureManager : public TextureManager
	{
	public:
		/** @copydoc TextureManager::getNativeFormat */
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) override;

	protected:		
		/** @copydoc TextureManager::createRenderTextureImpl */
		SPtr<RenderTexture> createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) override;
	};

	namespace ct
	{
	/**	Handles creation of Vulkan textures. */
	class VulkanTextureManager : public TextureManager
	{
	public:
		/** @copydoc TextureManager::onStartUp */
		void onStartUp() override;

		/** Returns a dummy (empty) texture that can be bound in a shader slot of the requested type. */
		VulkanTexture* getDummyTexture(GpuParamObjectType type) const;

		/**
		 * Determines Vulkan format required for binding a dummy texture (as returned by getDummyTexture()) to the shader
		 * expecting a format of type @p format.
		 */
		static VkFormat getDummyViewFormat(GpuBufferFormat format);
	protected:
		/** @copydoc TextureManager::createTextureInternal */
		SPtr<Texture> createTextureInternal(const TEXTURE_DESC& desc,
			const SPtr<PixelData>& initialData = nullptr, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		/** @copydoc TextureManager::createRenderTextureInternal */
		SPtr<RenderTexture> createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc,
			UINT32 deviceIdx = 0) override;

		SPtr<VulkanTexture> mDummyReadTextures[7];
		SPtr<VulkanTexture> mDummyStorageTextures[7];
	};
	}
	/** @} */
}

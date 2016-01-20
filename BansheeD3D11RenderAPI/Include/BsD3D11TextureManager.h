//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTextureManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of DirectX 11 textures.
	 */
	class BS_D3D11_EXPORT D3D11TextureManager : public TextureManager
	{
	public:
		/**
		 * @copydoc	TextureManager::getNativeFormat
		 */
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) override;

	protected:		
		/**
		 * @copydoc	TextureManager::createRenderTextureImpl
		 */
		RenderTexturePtr createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) override;

		/**
		 * @copydoc	TextureManager::createMultiRenderTextureImpl
		 */
		MultiRenderTexturePtr createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc) override;
	};

	/**
	 * @brief	Handles creation of DirectX 11 textures.
	 */
	class BS_D3D11_EXPORT D3D11TextureCoreManager : public TextureCoreManager
	{
	protected:
		/**
		 * @copydoc	TextureCoreManager::createTextureInternal
		 */
		SPtr<TextureCore> createTextureInternal(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int numMips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false,
			UINT32 multisampleCount = 0, const PixelDataPtr& initialData = nullptr) override;

		/**
		 * @copydoc	TextureCoreManager::createRenderTextureInternal
		 */
		SPtr<RenderTextureCore> createRenderTextureInternal(const RENDER_TEXTURE_CORE_DESC& desc) override;

		/**
		 * @copydoc	TextureCoreManager::createMultiRenderTextureInternal
		 */
		SPtr<MultiRenderTextureCore> createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_CORE_DESC& desc) override;
	};
}
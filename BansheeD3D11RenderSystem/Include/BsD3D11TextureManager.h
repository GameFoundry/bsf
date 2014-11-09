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
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma);

	protected:		
		/**
		 * @copydoc	TextureManager::createTextureImpl
		 */
		TexturePtr createTextureImpl();

		/**
		 * @copydoc	TextureManager::createRenderTextureImpl
		 */
		RenderTexturePtr createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	TextureManager::createMultiRenderTextureImpl
		 */
		MultiRenderTexturePtr createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc);
	};

	/**
	 * @brief	Handles creation of DirectX 11 textures.
	 */
	class BS_D3D11_EXPORT D3D11TextureCoreManager : public TextureCoreManager
	{
	protected:		
		/**
		 * @copydoc	TextureCoreManager::createRenderTextureInternal
		 */
		SPtr<RenderTextureCore> createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	TextureCoreManager::createMultiRenderTextureInternal
		 */
		SPtr<MultiRenderTextureCore> createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_DESC& desc);
	};
}
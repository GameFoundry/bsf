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
		D3D11TextureManager();
		~D3D11TextureManager();

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
		RenderTexturePtr createRenderTextureImpl();

		/**
		 * @copydoc	TextureManager::createMultiRenderTextureImpl
		 */
		MultiRenderTexturePtr createMultiRenderTextureImpl();
	};
}
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Texture.h"
#include "BsTextureManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of DirectX 9 textures.
	 */
	class BS_D3D9_EXPORT D3D9TextureManager : public TextureManager
	{
	public:
		D3D9TextureManager();
		~D3D9TextureManager();

		/**
		 * @copydoc TextureManager::getNativeFormat
		 */
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma);

	protected:	
		/**
		 * @copydoc TextureManager::createTextureImpl
		 */
		TexturePtr createTextureImpl();

		/**
		 * @copydoc TextureManager::createRenderTextureImpl
		 */
		RenderTexturePtr createRenderTextureImpl();

		/**
		 * @copydoc TextureManager::createMultiRenderTextureImpl
		 */
		MultiRenderTexturePtr createMultiRenderTextureImpl();
	};
}
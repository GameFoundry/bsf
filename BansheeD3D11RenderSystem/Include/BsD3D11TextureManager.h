#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsTextureManager.h"

namespace BansheeEngine 
{
	class BS_D3D11_EXPORT D3D11TextureManager : public TextureManager
	{
	public:
		D3D11TextureManager();
		~D3D11TextureManager();

		/// @copydoc TextureManager::getNativeFormat
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma);

	protected:		
		TexturePtr createTextureImpl();
		RenderTexturePtr createRenderTextureImpl();
		MultiRenderTexturePtr createMultiRenderTextureImpl();
	};
}
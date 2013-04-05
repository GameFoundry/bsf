#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTextureManager.h"

namespace CamelotEngine 
{
	class CM_D3D11_EXPORT D3D11TextureManager : public TextureManager
	{
	public:
		D3D11TextureManager();
		~D3D11TextureManager();

		/// @copydoc TextureManager::getNativeFormat
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);

	protected:		
		TexturePtr createTextureImpl();
		RenderTexturePtr createRenderTextureImpl();
		MultiRenderTexturePtr createMultiRenderTextureImpl();
	};
}
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

        /// @copydoc TextureManager::isHardwareFilteringSupported
        bool isHardwareFilteringSupported(TextureType ttype, PixelFormat format, int usage,
            bool preciseFormatOnly = false);		

		/**
		 * @copydoc TextureManager::createDepthStencilBuffer()
		 */
		DepthStencilBufferPtr createDepthStencilBuffer(DepthStencilFormat format, UINT32 width, 
			UINT32 height, UINT32 fsaa, const String& fsaaHint);

	protected:		
		Texture* createImpl();
	};
}
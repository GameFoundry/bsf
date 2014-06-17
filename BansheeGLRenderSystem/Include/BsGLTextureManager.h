#pragma once

#include "BsGLPrerequisites.h"
#include "BsGLTexture.h"
#include "BsGLSupport.h"
#include "BsTextureManager.h"

namespace BansheeEngine 
{
    /** GL-specific implementation of a TextureManager */
    class BS_RSGL_EXPORT GLTextureManager : public TextureManager
    {
    public:
        GLTextureManager(GLSupport& support);
        virtual ~GLTextureManager();

		/// @copydoc TextureManager::getNativeFormat
		PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage);

	protected:		
		TexturePtr createTextureImpl();
		RenderTexturePtr createRenderTextureImpl();
		MultiRenderTexturePtr createMultiRenderTextureImpl();

        GLSupport& mGLSupport;
    };
}

#pragma once

#include "BsGLPrerequisites.h"
#include "BsGLTexture.h"
#include "BsGLSupport.h"
#include "BsTextureManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of OpenGL textures.
	 */
    class BS_RSGL_EXPORT GLTextureManager : public TextureManager
    {
    public:
        GLTextureManager(GLSupport& support);
        virtual ~GLTextureManager();

		/**
		 * @brief	Converts the provided format for the specified texture type and usage
		 *			into a format that is supported by OpenGL.
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

        GLSupport& mGLSupport;
    };
}

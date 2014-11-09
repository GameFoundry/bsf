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
		RenderTexturePtr createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	TextureManager::createMultiRenderTextureImpl
		 */
		MultiRenderTexturePtr createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc);

        GLSupport& mGLSupport;
    };

	/**
	 * @brief	Handles creation of OpenGL textures.
	 */
	class BS_RSGL_EXPORT GLTextureCoreManager : public TextureCoreManager
	{
	public:
		GLTextureCoreManager(GLSupport& support);

	protected:		
		/**
		 * @copydoc	TextureCoreManager::createRenderTextureInternal
		 */
		SPtr<RenderTextureCore> createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	TextureCoreManager::createMultiRenderTextureInternal
		 */
		SPtr<MultiRenderTextureCore> createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_DESC& desc);

		GLSupport& mGLSupport;
	};
}

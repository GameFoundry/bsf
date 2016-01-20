//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

		GLSupport& mGLSupport;
	};
}

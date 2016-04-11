//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsTexture.h"
#include "BsRenderTexture.h"
#include "BsMultiRenderTexture.h"
#include "BsModule.h"

namespace BansheeEngine 
{
	/** @addtogroup Resources-Internal
	 *  @{
	 */

    /**
     * Defines interface for creation of textures. Render systems provide their own implementations.
	 *
	 * @note	Sim thread only.
     */
    class BS_CORE_EXPORT TextureManager : public Module<TextureManager>
    {
    public:
		virtual ~TextureManager() { }

		/** @copydoc Texture::create(TextureType, UINT32, UINT32, UINT32, int, PixelFormat, int, bool, UINT32) */
        SPtr<Texture> createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int numMips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, 
			UINT32 multisampleCount = 0);
			
		/** @copydoc Texture::create(TextureType, UINT32, UINT32, int, PixelFormat, int, bool, UINT32) */
		SPtr<Texture> createTexture(TextureType texType, UINT32 width, UINT32 height, int numMips,
			PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, UINT32 multisampleCount = 0)
		{
			return createTexture(texType, width, height, 1, 
				numMips, format, usage, hwGammaCorrection, multisampleCount);
		}

		/** @copydoc Texture::create(const SPtr<PixelData>&, int, bool) */
		SPtr<Texture> createTexture(const SPtr<PixelData>& pixelData, int usage = TU_DEFAULT, bool hwGammaCorrection = false);

		/**
		 * Creates a completely empty and uninitialized Texture.
		 *
		 * @note	
		 * Internal method. Should only be used for very specific purposes, like deserialization, as it requires additional
		 * manual initialization that is not required normally.
		 */
		SPtr<Texture> _createEmpty();

		/**
		 * Creates a new RenderTexture and automatically generates a color surface and (optionally) a depth/stencil surface.
		 *
		 * @param[in]	textureType			Type of the texture.
		 * @param[in]	width				Width of the texture in pixels.
		 * @param[in]	height				Height of the texture in pixels.
		 * @param[in]	format				Format of the pixels.
		 * @param[in]	hwGamma				If true, any color data will be gamma corrected before being written into the 
		 *									texture.
		 * @param[in]	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param[in]	createDepth			Determines will a depth/stencil buffer of the same size as the color buffer be
		 *									created for the render texture.
		 * @param[in]	depthStencilFormat	Format of the depth/stencil buffer if enabled.
		 */
		virtual SPtr<RenderTexture> createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format = PF_R8G8B8A8, bool hwGamma = false, UINT32 multisampleCount = 0, 
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		/** Creates a RenderTexture using the description struct. */
		virtual SPtr<RenderTexture> createRenderTexture(const RENDER_TEXTURE_DESC& desc);

		/** 
		 * Creates a new multi render texture. You may use this type of texture to render to multiple output textures at 
		 * once.
		 */
		virtual SPtr<MultiRenderTexture> createMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * Gets the format which will be natively used for a requested format given the constraints of the current device.
		 *
		 * @note	Thread safe.
		 */
		virtual PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) = 0;

	protected:
		/**
		 * Creates an empty and uninitialized render texture of a specific type. This is to be implemented by render 
		 * systems with their own implementations.
		 */
		virtual SPtr<RenderTexture> createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc) = 0;

		/**
		 * Creates an empty and uninitialized multi render texture of a specific type. This is to be implemented by render
		 * systems with their own implementations.
		 */
		virtual SPtr<MultiRenderTexture> createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc) = 0;

		mutable HTexture mDummyTexture;
    };

	/**
     * Defines interface for creation of textures. Render systems provide their own implementations.
	 *
	 * @note	Core thread only.
     */
    class BS_CORE_EXPORT TextureCoreManager : public Module<TextureCoreManager>
    {
    public:
		virtual ~TextureCoreManager() { }

		/** @copydoc Module::onStartUp */
		void onStartUp() override;

		/** @copydoc Module::onShutDown */
		void onShutDown() override;

		/**
		 * @copydoc	TextureManager::createTexture(TextureType, UINT32, UINT32, UINT32, int, PixelFormat, int, bool, UINT32)
		 */
		SPtr<TextureCore> createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int numMips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, 
			UINT32 multisampleCount = 0);

		/** @copydoc	TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&) */
		SPtr<RenderTextureCore> createRenderTexture(const RENDER_TEXTURE_CORE_DESC& desc);

		/** @copydoc	TextureManager::createMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC&) */
		SPtr<MultiRenderTextureCore> createMultiRenderTexture(const MULTI_RENDER_TEXTURE_CORE_DESC& desc);

	protected:
		friend class Texture;
		friend class TextureCore;
		friend class RenderTexture;
		friend class MultiRenderTexture;

		/**
		 * Creates an empty and uninitialized texture of a specific type. This is to be implemented	by render systems with
		 * their own implementations.
		 */
		virtual SPtr<TextureCore> createTextureInternal(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
			int numMips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false,
			UINT32 multisampleCount = 0, const SPtr<PixelData>& initialData = nullptr) = 0;

		/** @copydoc TextureManager::createRenderTextureImpl */
		virtual SPtr<RenderTextureCore> createRenderTextureInternal(const RENDER_TEXTURE_CORE_DESC& desc) = 0;

		/** @copydoc TextureManager::createMultiRenderTextureImpl */
		virtual SPtr<MultiRenderTextureCore> createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_CORE_DESC& desc) = 0;
    };

	/** @} */
}
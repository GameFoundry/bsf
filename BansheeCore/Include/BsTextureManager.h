//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"

#include "BsTexture.h"
#include "BsMultiRenderTexture.h"
#include "BsModule.h"

namespace BansheeEngine 
{
    /**
     * @brief	Defines interface for creation of textures. Render systems
	 *			provide their own implementations.
	 *
	 * @note	Thread safe.
     */
    class BS_CORE_EXPORT TextureManager : public Module<TextureManager>
    {
    public:
        TextureManager();
        virtual ~TextureManager();

		/**
		 * @copydoc	Texture::create(TextureType, UINT32, UINT32, UINT32, int, PixelFormat, int, bool, UINT32, const String&)
		 */
        TexturePtr createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, 
			int numMips, PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, 
			UINT32 multisampleCount = 0, const String& multisampleHint = StringUtil::BLANK);
			
		/**
		 * @copydoc	Texture::create(TextureType, UINT32, UINT32, int, PixelFormat, int, bool, UINT32, const String&)
		 */
		TexturePtr createTexture(TextureType texType, UINT32 width, UINT32 height, int numMips,
			PixelFormat format, int usage = TU_DEFAULT, bool hwGammaCorrection = false, UINT32 multisampleCount = 0,
			const String& multisampleHint = StringUtil::BLANK)
		{
			return createTexture(texType, width, height, 1, 
				numMips, format, usage, hwGammaCorrection, multisampleCount, multisampleHint);
		}

		/**
		 * @brief	Creates a completely empty and uninitialized Texture.
		 *
		 * @note	Internal method. Should only be used for very specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		TexturePtr _createEmpty();

		/**
		 * @brief	Creates a new RenderTexture and automatically generates a color surface
		 * 			and (optionally) a depth/stencil surface.
		 *
		 * @param	texType				Type of the texture.
		 * @param	width				Width of the texture in pixels.
		 * @param	height				Height of the texture in pixels.
		 * @param	format				Format of the pixels.
		 * @param	hwGamma				If true, any color data will be gamma corrected before being written
		 *								into the texture.
		 * @param	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param	multisampleHint		Hint about what kind of multisampling to use. Render system specific.
		 * @param	createDepth			Determines will a depth/stencil buffer of the same size as the color buffer be created
		 *								for the render texture.
		 * @param	depthStencilFormat	Format of the depth/stencil buffer if enabled.
		 */
		virtual RenderTexturePtr createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format = PF_R8G8B8A8, bool hwGamma = false, UINT32 multisampleCount = 0, 
			const String& multisampleHint = "", bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		/**
		 * @brief	Creates a RenderTexture using the description struct.
		 */
		virtual RenderTexturePtr createRenderTexture(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @brief	Creates a new multi render texture. You may use this type of texture
		 * 			to render to multiple output textures at once.
		 */
		virtual MultiRenderTexturePtr createMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @brief	Gets the format which will be natively used for a requested format given the
		 *			constraints of the current device.
		 *
		 * @note	Thread safe.
		 */
		virtual PixelFormat getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma) = 0;

		/**
		 * @brief	Returns tiny dummy texture for use when no other is available.
		 */
		const HTexture& getDummyTexture() const { return mDummyTexture; }

	protected:
		/**
		 * @brief	Creates an empty and uninitialized texture of a specific type. This is to be implemented
		 *			by render systems with their own implementations.
		 */
		virtual TexturePtr createTextureImpl() = 0;

		/**
		 * @brief	Creates an empty and uninitialized render texture of a specific type. This 
		 *			is to be implemented by render systems with their own implementations.
		 */
		virtual RenderTexturePtr createRenderTextureImpl() = 0;

		/**
		 * @brief	Creates an empty and uninitialized multi render texture of a specific type. This is 
		 *			to be implemented by render systems with their own implementations.
		 */
		virtual MultiRenderTexturePtr createMultiRenderTextureImpl() = 0;

		/**
		 * @copydoc	Module::onStartUp
		 */
		virtual void onStartUp();

	protected:
		HTexture mDummyTexture;
    };
}
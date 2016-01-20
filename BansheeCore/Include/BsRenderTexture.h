//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsTexture.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{    
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/**	Structure that describes a render texture color and depth/stencil surfaces. */
	struct BS_CORE_EXPORT RENDER_TEXTURE_DESC
	{
		RENDER_SURFACE_DESC colorSurface;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	struct RENDER_TEXTURE_CORE_DESC;

	/**	Contains various properties that describe a render texture. */
	class BS_CORE_EXPORT RenderTextureProperties : public RenderTargetProperties
	{
	public:
		RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping);
		RenderTextureProperties(const RENDER_TEXTURE_CORE_DESC& desc, bool requiresFlipping);
		virtual ~RenderTextureProperties() { }

	private:
		void construct(const TextureProperties* textureProps, bool requiresFlipping);

		friend class RenderTextureCore;
		friend class RenderTexture;
	};

	/** @cond INTERNAL */

	/**
	 * @see		RENDER_TEXTURE_DESC
	 *
	 * @note	References core textures instead of texture handles.
	 */
	struct BS_CORE_EXPORT RENDER_TEXTURE_CORE_DESC
	{
		RENDER_SURFACE_CORE_DESC colorSurface;
		RENDER_SURFACE_CORE_DESC depthStencilSurface;
	};

	/**
	 * Provides access to internal render texture implementation usable only from the core thread.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderTextureCore : public RenderTargetCore
	{
	public:
		RenderTextureCore(const RENDER_TEXTURE_CORE_DESC& desc);
		virtual ~RenderTextureCore();

		/** @copydoc CoreObjectCore::initialize */
		virtual void initialize();

		/**
		 * Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		SPtr<TextureCore> getBindableColorTexture() const { return mDesc.colorSurface.texture; }

		/**
		 * Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		SPtr<TextureCore> getBindableDepthStencilTexture() const { return mDesc.depthStencilSurface.texture; }

		/**	Returns properties that describe the render texture. */
		const RenderTextureProperties& getProperties() const;

	protected:
		/** @copydoc CoreObjectCore::syncToCore */
		virtual void syncToCore(const CoreSyncData& data) override;

	private:
		/**	Throws an exception of the color and depth/stencil buffers aren't compatible. */
		void throwIfBuffersDontMatch() const;

	protected:
		friend class RenderTexture;

		TextureViewPtr mColorSurface;
		TextureViewPtr mDepthStencilSurface;

		RENDER_TEXTURE_CORE_DESC mDesc;
	};

	/** @endcond */

	/**
	 * Render target specialization that allows you to render into a texture you may later bind in further render operations.
	 *
	 * @note	Sim thread only. Retrieve core implementation from getCore() for core thread only functionality.
	 */
    class BS_CORE_EXPORT RenderTexture : public RenderTarget
    {
	public:
		virtual ~RenderTexture() { }

		/**
		 * Creates a new render texture with color and optionally depth/stencil surfaces.
		 *
		 * @param[in]	textureType			Type of texture to render to.
		 * @param[in]	width				Width of the render texture, in pixels.
		 * @param[in]	height				Height of the render texture, in pixels.
		 * @param[in]	format				Pixel format used by the texture color surface.
		 * @param[in]	hwGamma				Should the written pixels be gamma corrected.
		 * @param[in]	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param[in]	createDepth			Should a depth/stencil surface be created along with the color surface.
		 * @param[in]	depthStencilFormat	Format used by the depth stencil surface, if one is created.
		 */
		static RenderTexturePtr create(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format = PF_R8G8B8A8, bool hwGamma = false, UINT32 multisampleCount = 0, 
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		/** @copydoc TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&) */
		static RenderTexturePtr create(const RENDER_TEXTURE_DESC& desc);

		/**
		 * Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getBindableColorTexture() const { return mBindableColorTex; }

		/**
		 * Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getBindableDepthStencilTexture() const { return mBindableDepthStencilTex; }

		/**
		 * Retrieves a core implementation of a render texture usable only from the core thread.
		 *
		 * @note	Core thread only.
		 */
		SPtr<RenderTextureCore> getCore() const;

		/**	Returns properties that describe the render texture. */
		const RenderTextureProperties& getProperties() const;

	protected:
		friend class TextureManager;

		RenderTexture(const RENDER_TEXTURE_DESC& desc);

		/** @copydoc RenderTexture::createCore */
		virtual SPtr<CoreObjectCore> createCore() const override;

		/** @copydoc CoreObjectCore::syncToCore */
		virtual CoreSyncData syncToCore(FrameAlloc* allocator) override;

	protected:
		HTexture mBindableColorTex;
		HTexture mBindableDepthStencilTex;

		RENDER_TEXTURE_DESC mDesc;
	};

	/** @} */
}
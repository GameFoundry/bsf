#pragma once

#include "BsCorePrerequisites.h"
#include "BsTexture.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{    
	/**
	 * @brief	Structure that describes a render texture color and depth/stencil surfaces.
	 */
	struct BS_CORE_EXPORT RENDER_TEXTURE_DESC
	{
		RENDER_SURFACE_DESC colorSurface;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	/**
	 * @brief	Contains various properties that describe a render texture.
	 */
	class BS_CORE_EXPORT RenderTextureProperties : public RenderTargetProperties
	{
	public:
		virtual ~RenderTextureProperties() { }

		/**
		 * @copydoc	RenderTargetProperties::copyFrom
		 */
		virtual void copyFrom(const RenderTargetProperties& other);

	private:
		friend class RenderTextureCore;
		friend class RenderTexture;
	};

	/**
	 * @brief	Provides access to internal render texture implementation usable only from the core thread.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT RenderTextureCore : public RenderTargetCore
	{
	public:
		RenderTextureCore(RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc);
		virtual ~RenderTextureCore();

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const RenderTextureProperties& getProperties() const { return *static_cast<RenderTextureProperties*>(mProperties); }

		/**
		 * @copydoc	RenderTargetCore::getNonCore
		 */
		RenderTexture* getNonCore() const;

	private:
		/**
		 * @brief	Throws an exception of the color and depth/stencil buffers aren't compatible.
		 */
		void throwIfBuffersDontMatch() const;

	protected:
		friend class RenderTexture;

		TextureViewPtr mColorSurface;
		TextureViewPtr mDepthStencilSurface;
	};

	/**
	 * @brief	Render target specialization that allows you to render into a texture you may
	 *			later bind in further render operations.
	 *
	 * @note	Sim thread only. Retrieve core implementation from getCore()
	 *			for core thread only functionality.
	 */
    class BS_CORE_EXPORT RenderTexture : public RenderTarget
    {
	public:
		virtual ~RenderTexture() { }

		/**
		 * @brief	Creates a new render texture with color and optionally depth/stencil surfaces.
		 *
		 * @param	textureType			Type of texture to render to.
		 * @param	width				Width of the render texture, in pixels.
		 * @param	height				Height of the render texture, in pixels.
		 * @param	format				Pixel format used by the texture color surface.
		 * @param	hwGamma				Should the written pixels be gamma corrected.
		 * @param	multisampleCount	If higher than 1, texture containing multiple samples per pixel is created.
		 * @param	createDepth			Should a depth/stencil surface be created along with the color surface.
		 * @param	depthStencilFormat	Format used by the depth stencil surface, if one is created.
		 */
		static RenderTexturePtr create(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format = PF_R8G8B8A8, bool hwGamma = false, UINT32 multisampleCount = 0, 
			bool createDepth = true, PixelFormat depthStencilFormat = PF_D24S8);

		/**
		 * @copydoc	TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC&)
		 */
		static RenderTexturePtr create(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTexture::requiresTextureFlipping
		 */
		virtual bool requiresTextureFlipping() const { return false; }

		/**
		 * @brief	Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getBindableColorTexture() const { return mBindableColorTex; }

		/**
		 * @brief	Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note		Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const HTexture& getBindableDepthStencilTexture() const { return mBindableDepthStencilTex; }

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const RenderTextureProperties& getProperties() const;

		/**
		 * @brief	Retrieves a core implementation of a render texture usable only from the
		 *			core thread.
		 *
		 * @note	Core thread only.
		 */
		RenderTextureCore* getCore() const;

	protected:
		friend class TextureManager;

		RenderTexture() { }

		/**
		 * @copydoc	RenderTarget::initialize
		 */
		virtual void initialize(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual RenderTargetCore* createCore();

		/**
		 * @brief	Creates a core implementation of a render texture. This implementation
		 *			is to be used on the core thread only.
		 */
		virtual RenderTextureCore* createCore(RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc) = 0;

	protected:
		HTexture mBindableColorTex;
		HTexture mBindableDepthStencilTex;

		RENDER_SURFACE_DESC mColorSurfaceDesc;
		RENDER_SURFACE_DESC mDepthStencilSurfaceDesc;
	};
}
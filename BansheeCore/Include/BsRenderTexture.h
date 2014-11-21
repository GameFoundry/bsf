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
		RenderTextureProperties(const RENDER_TEXTURE_DESC& desc, bool requiresFlipping);
		virtual ~RenderTextureProperties() { }

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
		RenderTextureCore(const RENDER_TEXTURE_DESC& desc);
		virtual ~RenderTextureCore();

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @brief	Returns a color surface texture you may bind as an input to an GPU program.
		 *
		 * @note	Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const TexturePtr& getBindableColorTexture() const { return mDesc.colorSurface.texture; }

		/**
		 * @brief	Returns a depth/stencil surface texture you may bind as an input to an GPU program.
		 *
		 * @note		Be aware that you cannot bind a render texture for reading and writing at the same time.
		 */
		const TexturePtr& getBindableDepthStencilTexture() const { return mDesc.depthStencilSurface.texture; }

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const RenderTextureProperties& getProperties() const;

	protected:
		/**
		 * @copydoc	CoreObjectCore::syncFromCore
		 */
		virtual CoreSyncData syncFromCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual void syncToCore(const CoreSyncData& data);

	private:
		/**
		 * @brief	Throws an exception of the color and depth/stencil buffers aren't compatible.
		 */
		void throwIfBuffersDontMatch() const;

	protected:
		friend class RenderTexture;

		TextureViewPtr mColorSurface;
		TextureViewPtr mDepthStencilSurface;

		RENDER_TEXTURE_DESC mDesc;
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
		 * @brief	Retrieves a core implementation of a render texture usable only from the
		 *			core thread.
		 *
		 * @note	Core thread only.
		 */
		SPtr<RenderTextureCore> getCore() const;

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const RenderTextureProperties& getProperties() const;

	protected:
		friend class TextureManager;

		RenderTexture(const RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual SPtr<CoreObjectCore> createCore() const;

		/**
		 * @copydoc	CoreObjectCore::syncToCore
		 */
		virtual CoreSyncData syncToCore(FrameAlloc* allocator);

		/**
		 * @copydoc	CoreObjectCore::syncFromCore
		 */
		virtual void syncFromCore(const CoreSyncData& data);

	protected:
		HTexture mBindableColorTex;
		HTexture mBindableDepthStencilTex;

		RENDER_TEXTURE_DESC mDesc;
	};
}
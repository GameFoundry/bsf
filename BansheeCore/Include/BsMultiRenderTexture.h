#pragma once

#include "BsCorePrerequisites.h"
#include "BsRenderTarget.h"

namespace BansheeEngine
{
	/**
	 * @brief	Descriptor class used for initializing a MultiRenderTexture.
	 *			Contains descriptors for each individual color render surface and 
	 *			their common depth/stencil surface.
	 */
	struct BS_CORE_EXPORT MULTI_RENDER_TEXTURE_DESC
	{
		Vector<RENDER_SURFACE_DESC> colorSurfaces;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	/**
	 * @brief	Contains various properties that describe a render texture.
	 */
	class BS_CORE_EXPORT MultiRenderTextureProperties : public RenderTargetProperties
	{
	public:
		virtual ~MultiRenderTextureProperties() { }

	private:
		friend class MultiRenderTextureCore;
		friend class MultiRenderTexture;
	};

	/**
	 * @brief	Object representing multiple render textures. You may bind this to the pipeline
	 *			in order to render to all or some of the textures at once.
	 *
	 * @note	Core thread only.
	 */
	class BS_CORE_EXPORT MultiRenderTextureCore : public RenderTargetCore
	{
	public:
		virtual ~MultiRenderTextureCore();

	protected:
		MultiRenderTextureCore(MultiRenderTexture* parent, MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTargetCore::getNonCore
		 */
		MultiRenderTexture* getNonCore() const;

	private:
		/**
		 * @brief	Checks that all render surfaces and depth/stencil surface match. If they do not match
		 *			an exception is thrown.
		 */
		void throwIfBuffersDontMatch() const;

		// TODO - Not implemented
		virtual void copyToMemory(PixelData &dst, FrameBuffer buffer = FB_AUTO);

	protected:
		Vector<TextureViewPtr> mColorSurfaces;
		TextureViewPtr mDepthStencilSurface;
	};

	/**
	 * @brief	Object representing multiple render textures. You may bind this to the pipeline
	 *			in order to render to all or some of the textures at once.
	 *
	 * @note	Sim thread only.
	 */
	class BS_CORE_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture() { }

		/**
		 * @copydoc	RenderTarget::initialize
		 */
		void initialize(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc	RenderTexture::requiresTextureFlipping
		 */
		virtual bool requiresTextureFlipping() const { return false; }

		/**
		 * @brief	Returns properties that describe the render texture.
		 */
		const MultiRenderTextureProperties& getProperties() const;

		/**
		 * @brief	Retrieves a core implementation of a render texture usable only from the
		 *			core thread.
		 *
		 * @note	Core thread only.
		 */
		MultiRenderTextureCore* getCore() const;

	protected:
		MultiRenderTexture() { }

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual RenderTargetCore* createCore();

		/**
		 * @brief	Creates a core implementation of a render texture. This implementation
		 *			is to be used on the core thread only.
		 */
		virtual MultiRenderTextureCore* createCore(MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc) = 0;

		MULTI_RENDER_TEXTURE_DESC mDesc;
	};
}
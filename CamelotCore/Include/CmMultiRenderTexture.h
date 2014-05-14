#pragma once

#include "CmPrerequisites.h"
#include "CmRenderTarget.h"

namespace BansheeEngine
{
	/**
	 * @brief	Descriptor class used for initializing a MultiRenderTexture.
	 *			Contains descriptors for each individual color render surface and 
	 *			their common depth/stencil surface.
	 */
	struct CM_EXPORT MULTI_RENDER_TEXTURE_DESC
	{
		Vector<RENDER_SURFACE_DESC> colorSurfaces;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	/**
	 * @brief	Object representing multiple render textures. You may bind this to the pipeline
	 *			in order to render to all or some of the textures at once.
	 */
	class CM_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture();

		/**
		 * @copydoc	RenderTarget::initialize
		 */
		void initialize(const MULTI_RENDER_TEXTURE_DESC& desc);

		/**
		 * @copydoc RenderTarget::isWindow.
		 */
		bool isWindow() const { return true; }

		/**
		 * @copydoc RenderTarget::requiresTextureFlipping.
		 */
		bool requiresTextureFlipping() const { return false; }

	protected:
		MultiRenderTexture();

		/**
		 * @copydoc RenderTarget::destroy_internal()
		 */
		virtual void destroy_internal();

	private:
		/**
		 * @brief	Checks that all render surfaces and depth/stencil surface match. If they do not match
		 *			an exception is thrown.
		 */
		void throwIfBuffersDontMatch() const;

		// TODO - Not implemented
		virtual void copyToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO);

	protected:
		Vector<TextureViewPtr> mColorSurfaces;
		TextureViewPtr mDepthStencilSurface;
	};
}
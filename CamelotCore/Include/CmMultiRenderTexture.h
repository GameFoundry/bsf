#pragma once

#include "CmPrerequisites.h"
#include "CmRenderTarget.h"

namespace CamelotFramework
{
	struct CM_EXPORT MULTI_RENDER_TEXTURE_DESC
	{
		Vector<RENDER_SURFACE_DESC>::type colorSurfaces;
		RENDER_SURFACE_DESC depthStencilSurface;
	};

	class CM_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture();

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
		Vector<TextureViewPtr>::type mColorSurfaces;
		TextureViewPtr mDepthStencilSurface;

		MultiRenderTexture();

		/**
		 * @copydoc RenderTarget::destroy_internal()
		 */
		virtual void destroy_internal();

	private:
		void throwIfBuffersDontMatch() const;

		virtual void copyToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO);
	};
}
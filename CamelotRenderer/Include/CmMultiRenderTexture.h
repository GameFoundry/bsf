#pragma once

#include "CmPrerequisites.h"
#include "CmRenderTarget.h"

namespace CamelotEngine
{
	class CM_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture() {}

		void setColorSurface(UINT32 surfaceIdx, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 0, UINT32 mipLevel = 0);
		void setDepthStencil(DepthStencilBufferPtr depthStencilbuffer);

		SurfaceDesc getSurfaceDesc(UINT32 surfaceIdx) const { return mSurfaces.at(surfaceIdx); }
		DepthStencilBufferPtr getDepthStencilBuffer() const { return mDepthStencilBuffer; }

		bool requiresTextureFlipping() const { return false; }

	protected:
		vector<SurfaceDesc>::type mSurfaces;
		DepthStencilBufferPtr mDepthStencilBuffer;

		MultiRenderTexture();
		virtual void initialize() {}

		virtual void setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 0, UINT32 mipLevel = 0) = 0;
		virtual void setDepthStencilImpl(DepthStencilBufferPtr depthStencilbuffer) = 0;
	private:
		void throwIfBuffersDontMatch() const;

		virtual void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer = FB_AUTO);
	};
}
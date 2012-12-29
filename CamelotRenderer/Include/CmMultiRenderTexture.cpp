#pragma once

#include "CmPrerequisites.h"
#include "CmRenderTarget.h"

namespace CamelotEngine
{
	class CM_EXPORT MultiRenderTexture : public RenderTarget
	{
	public:
		virtual ~MultiRenderTexture() {}

		void setColorSurface(UINT32 surfaceIndex, TexturePtr texture, UINT32 face = 0, UINT32 numFaces = 0, UINT32 mipLevel = 0);
		void setDepthStencil(DepthStencilBufferPtr depthStencilbuffer);

		TexturePtr getTexture(UINT32 index) const { return mTextures.at(index); }
		DepthStencilBufferPtr getDepthStencilBuffer() const { return mDepthStencilBuffer; }

		bool requiresTextureFlipping() const { return false; }

	protected:
		vector<TexturePtr>::type mTextures;
		DepthStencilBufferPtr mDepthStencilBuffer;

		MultiRenderTexture();

		void initialize();

		void createInternalResources();
		virtual void createInternalResourcesImpl() = 0;
	};
}
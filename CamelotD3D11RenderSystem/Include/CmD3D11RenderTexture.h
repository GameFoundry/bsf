#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmTexture.h"
#include "CmRenderTexture.h"

namespace CamelotEngine
{
	class D3D11RenderTexture : public RenderTexture
	{
	public:
		D3D11RenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format, bool hwGamma, UINT32 fsaa, const String& fsaaHint, 
			bool createDepth = true, UINT32 depthBits = 32);
		D3D11RenderTexture(TexturePtr texture, DepthStencilBufferPtr depthStencilbuffer);
		virtual ~D3D11RenderTexture();

		void setBuffers(TexturePtr texture, DepthStencilBufferPtr depthStencilbuffer);

		TexturePtr getTexture() const { return mTexture; }
		DepthStencilBufferPtr getDepthStencilBuffer() const { return mDepthStencilBuffer; }

		bool requiresTextureFlipping() const { return false; }

	protected:
		TextureType mType;
		PixelFormat mFormat;
		UINT32 mDepthBits;

		TexturePtr mTexture;
		DepthStencilBufferPtr mDepthStencilBuffer;
		ID3D11RenderTargetView* mRenderTargetView;

		void createTextureBuffer();
		void createDepthStencilBuffer();
		void createResourceView();
	};
}
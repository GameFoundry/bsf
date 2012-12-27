#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderTexture.h"

namespace CamelotEngine
{
	class D3D11RenderTexture : public RenderTexture
	{
		D3D11Device& mDevice;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
	public:
		D3D11RenderTexture(const String &name, D3D11HardwarePixelBuffer *buffer, D3D11Device& device);
		virtual ~D3D11RenderTexture();

		void rebind(D3D11HardwarePixelBuffer *buffer);

		virtual void getCustomAttribute(const String& name, void *pData);

		bool requiresTextureFlipping() const { return false; }
	};
}
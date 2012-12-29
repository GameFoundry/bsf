#include "CmD3D11DepthStencilBuffer.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Mappings.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11DepthStencilBuffer::D3D11DepthStencilBuffer(DepthStencilFormat format, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		: DepthStencilBuffer(format, width, height, fsaa, fsaaHint)
		, mDepthStencil(nullptr)
		, mDepthStencilView(nullptr)
	{
		// Create depth stencil texture
		D3D11_TEXTURE2D_DESC descDepth;

		descDepth.Width = width;
		descDepth.Height = height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = D3D11Mappings::get(format);
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		rs->determineFSAASettings(fsaa, fsaaHint, descDepth.Format, &descDepth.SampleDesc);

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateTexture2D(&descDepth, NULL, &mDepthStencil);
		if( FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Unable to create depth texture\nError Description:" + errorDescription);
		}

		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		descDSV.Format =  descDepth.Format;
		descDSV.ViewDimension = (fsaa != 0) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = device.getD3D11Device()->CreateDepthStencilView(mDepthStencil, &descDSV, &mDepthStencilView);

		if(FAILED(hr))
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Unable to create depth stencil view\nError Description:" + errorDescription);
		}
	}

	D3D11DepthStencilBuffer::~D3D11DepthStencilBuffer()
	{
		SAFE_RELEASE(mDepthStencilView);
		SAFE_RELEASE(mDepthStencil);
	}

	bool D3D11DepthStencilBuffer::isCompatible(RenderTarget* renderTarget) const
	{
		// Implement once I have RenderTarget properly implemented
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}
}
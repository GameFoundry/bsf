#include "CmD3D11RenderTexture.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Texture.h"
#include "CmD3D11Mappings.h"
#include "CmTextureManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11RenderTexture::D3D11RenderTexture()
		:RenderTexture(), mRenderTargetView(nullptr)
	{

	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
		SAFE_RELEASE(mRenderTargetView);
		SAFE_RELEASE(mDepthStencilView);
	}

	ID3D11RenderTargetView* D3D11RenderTexture::createRenderTargetView(const SurfaceDesc& surfaceDesc)
	{
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));

		D3D11Texture* d3d11Texture = static_cast<D3D11Texture*>(surfaceDesc.texture.get());

		RTVDesc.Format = d3d11Texture->getDXGIFormat();
		switch(d3d11Texture->getDimension())
		{
		case D3D11_SRV_DIMENSION_BUFFER:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			RTVDesc.Texture1D.MipSlice = surfaceDesc.mipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			RTVDesc.Texture1DArray.FirstArraySlice = surfaceDesc.face;
			RTVDesc.Texture1DArray.ArraySize = surfaceDesc.numFaces;
			RTVDesc.Texture1DArray.MipSlice = surfaceDesc.mipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.FirstArraySlice = surfaceDesc.face;
			RTVDesc.Texture2DArray.ArraySize = surfaceDesc.numFaces;
			RTVDesc.Texture2DArray.MipSlice = surfaceDesc.mipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RTVDesc.Texture2D.MipSlice = surfaceDesc.mipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.FirstArraySlice = surfaceDesc.face;
			RTVDesc.Texture2DArray.ArraySize = surfaceDesc.numFaces;
			RTVDesc.Texture2DArray.MipSlice = surfaceDesc.mipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			RTVDesc.Texture2DMSArray.FirstArraySlice = surfaceDesc.face;
			RTVDesc.Texture2DMSArray.ArraySize = surfaceDesc.numFaces;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE3D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			RTVDesc.Texture3D.MipSlice = surfaceDesc.mipLevel;
			RTVDesc.Texture3D.FirstWSlice = surfaceDesc.face;
			RTVDesc.Texture3D.WSize = surfaceDesc.numFaces;
			break;
		default:
			assert(false);
		}

		ID3D11RenderTargetView* rtv;
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRenderTargetView(d3d11Texture->getDX11Resource(), &RTVDesc, &rtv);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating Render Target View\nError Description:" + errorDescription);
		}

		return rtv;
	}

	ID3D11DepthStencilView* D3D11RenderTexture::createDepthStencilView(const TexturePtr& depthStencilSurface)
	{
		// Create the depth stencil view
		D3D11_DEPTH_STENCIL_VIEW_DESC DSVDesc;
		ZeroMemory(&DSVDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

		D3D11Texture* d3d11Texture = static_cast<D3D11Texture*>(depthStencilSurface.get());

		DSVDesc.Format = d3d11Texture->getDXGIFormat();
		switch(d3d11Texture->getDimension())
		{
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			DSVDesc.Texture1D.MipSlice = 0;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1DARRAY;
			DSVDesc.Texture1DArray.FirstArraySlice = 0;
			DSVDesc.Texture1DArray.ArraySize = 1;
			DSVDesc.Texture1DArray.MipSlice = 0;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			DSVDesc.Texture2D.MipSlice = 0;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			DSVDesc.Texture2DArray.FirstArraySlice = 0;
			DSVDesc.Texture2DArray.ArraySize = 1;
			DSVDesc.Texture2DArray.MipSlice = 0;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			DSVDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY;
			DSVDesc.Texture2DMSArray.FirstArraySlice = 0;
			DSVDesc.Texture2DMSArray.ArraySize = 1;
			break;
		default:
			assert(false);
		}

		ID3D11DepthStencilView* dsv;
		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		D3D11Device& device = rs->getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateDepthStencilView(d3d11Texture->getDX11Resource(), &DSVDesc, &dsv);

		if(FAILED(hr))
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Unable to create depth stencil view\nError Description:" + errorDescription);
		}

		return dsv;
	}

	void D3D11RenderTexture::createInternalResourcesImpl()
	{
		SAFE_RELEASE(mRenderTargetView);
		SAFE_RELEASE(mDepthStencilView);

		mRenderTargetView = createRenderTargetView(mSurface);
		mDepthStencilView = createDepthStencilView(mDepthStencilSurface);
	}

	void D3D11RenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** pRTVs = (ID3D11RenderTargetView **)pData;						
			*pRTVs = mRenderTargetView;		

			return;
		}
		else if(name == "DSV")
		{
			ID3D11DepthStencilView** pDSV = (ID3D11DepthStencilView **)pData;
			*pDSV = mDepthStencilView;
			return;
		}
	}
}
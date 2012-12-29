#include "CmD3D11RenderTexture.h"
#include "CmDepthStencilBuffer.h"
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
	}

	void D3D11RenderTexture::createInternalResourcesImpl()
	{
		SAFE_RELEASE(mRenderTargetView);

		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory(&RTVDesc, sizeof(RTVDesc));

		D3D11Texture* d3d11Texture = static_cast<D3D11Texture*>(mTexture.get());

		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = d3d11Texture->getSRVDesc();
		RTVDesc.Format = SRVDesc.Format;

		switch(SRVDesc.ViewDimension)
		{
		case D3D11_SRV_DIMENSION_BUFFER:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_BUFFER;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			RTVDesc.Texture1D.MipSlice = mMipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
			RTVDesc.Texture1DArray.FirstArraySlice = mFace;
			RTVDesc.Texture1DArray.ArraySize = mNumFaces;
			RTVDesc.Texture1DArray.MipSlice = mMipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURECUBE:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.FirstArraySlice = mFace;
			RTVDesc.Texture2DArray.ArraySize = mNumFaces;
			RTVDesc.Texture2DArray.MipSlice = mMipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			RTVDesc.Texture2D.MipSlice = mMipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			RTVDesc.Texture2DArray.FirstArraySlice = mFace;
			RTVDesc.Texture2DArray.ArraySize = mNumFaces;
			RTVDesc.Texture2DArray.MipSlice = mMipLevel;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMS:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			RTVDesc.Texture2DMSArray.FirstArraySlice = mFace;
			RTVDesc.Texture2DMSArray.ArraySize = mNumFaces;
			break;
		case D3D11_SRV_DIMENSION_TEXTURE3D:
			RTVDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			RTVDesc.Texture3D.MipSlice = mMipLevel;
			RTVDesc.Texture3D.FirstWSlice = mFace;
			RTVDesc.Texture3D.WSize = mNumFaces;
			break;
		default:
			assert(false);
		}

		D3D11Device& device = D3D11RenderSystem::getPrimaryDevice();
		HRESULT hr = device.getD3D11Device()->CreateRenderTargetView(d3d11Texture->getDX11Resource(), &RTVDesc, &mRenderTargetView);

		if (FAILED(hr) || device.hasError())
		{
			String errorDescription = device.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Error creating Render Target View\nError Description:" + errorDescription);
		}
	}
}
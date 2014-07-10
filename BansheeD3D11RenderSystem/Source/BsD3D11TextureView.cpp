//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11TextureView.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsD3D11Texture.h"
#include "BsUtil.h"
#include "BsRenderStats.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D11TextureView::D3D11TextureView()
		:TextureView(), mSRV(nullptr), mUAV(nullptr), mDSV(nullptr), mRTV(nullptr)
	{

	}

	void D3D11TextureView::initialize_internal()
	{
		D3D11Texture* d3d11Texture = static_cast<D3D11Texture*>(mOwnerTexture.get());

		if((mDesc.usage & GVU_RANDOMWRITE) != 0)
			mUAV = createUAV(d3d11Texture, mDesc.mostDetailMip, mDesc.firstArraySlice, mDesc.numArraySlices);
		else if((mDesc.usage & GVU_RENDERTARGET) != 0)
			mRTV = createRTV(d3d11Texture, mDesc.mostDetailMip, mDesc.firstArraySlice, mDesc.numArraySlices);
		else if((mDesc.usage & GVU_DEPTHSTENCIL) != 0)
			mDSV = createDSV(d3d11Texture, mDesc.mostDetailMip, mDesc.firstArraySlice, mDesc.numArraySlices);
		else
			mSRV = createSRV(d3d11Texture, mDesc.mostDetailMip, mDesc.numMips, mDesc.firstArraySlice, mDesc.numArraySlices);

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_ResourceView);
		TextureView::initialize_internal();
	}

	void D3D11TextureView::destroy_internal()
	{
		SAFE_RELEASE(mSRV);
		SAFE_RELEASE(mUAV);
		SAFE_RELEASE(mDSV);
		SAFE_RELEASE(mRTV);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_ResourceView);
		TextureView::destroy_internal();
	}

	ID3D11ShaderResourceView* D3D11TextureView::createSRV(D3D11Texture* texture, 
		UINT32 mostDetailMip, UINT32 numMips, UINT32 firstArraySlice, UINT32 numArraySlices)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		switch(texture->getTextureType())
		{
		case TEX_TYPE_1D:
			desc.Texture1D.MipLevels = numMips;
			desc.Texture1D.MostDetailedMip = mostDetailMip;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
			break;
		case TEX_TYPE_2D:
			desc.Texture2D.MipLevels = numMips;
			desc.Texture2D.MostDetailedMip = mostDetailMip;
			if(texture->getMultisampleCount() > 0)
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			else
				desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			break;
		case TEX_TYPE_3D:
			desc.Texture3D.MipLevels = numMips;
			desc.Texture3D.MostDetailedMip = mostDetailMip;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
			break;
		case TEX_TYPE_CUBE_MAP:
			desc.TextureCube.MipLevels = numMips;
			desc.TextureCube.MostDetailedMip = mostDetailMip;
			desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Invalid texture type for this view type.");
		}

		desc.Format = texture->getDXGIFormat();

		ID3D11ShaderResourceView* srv = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateShaderResourceView(texture->getDX11Resource(), &desc, &srv);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create ShaderResourceView: " + msg);
		}

		return srv;
	}

	ID3D11RenderTargetView* D3D11TextureView::createRTV(D3D11Texture* texture, 
		UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices)
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		switch(texture->getTextureType())
		{
		case TEX_TYPE_1D:
			desc.Texture1D.MipSlice = mipSlice;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			break;
		case TEX_TYPE_2D:
			desc.Texture2D.MipSlice = mipSlice;
			if(texture->getMultisampleCount() > 0)
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
			else
				desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			break;
		case TEX_TYPE_3D:
			desc.Texture3D.MipSlice = mipSlice;
			desc.Texture3D.FirstWSlice = firstArraySlice;
			desc.Texture3D.WSize = numArraySlices;
			desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Invalid texture type for this view type.");
		}

		desc.Format = texture->getDXGIFormat();

		ID3D11RenderTargetView* rtv = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateRenderTargetView(texture->getDX11Resource(), &desc, &rtv);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create RenderTargetView: " + msg);
		}

		return rtv;
	}

	ID3D11UnorderedAccessView* D3D11TextureView::createUAV(D3D11Texture* texture,
		UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices)
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		switch(texture->getTextureType())
		{
		case TEX_TYPE_1D:
			desc.Texture1D.MipSlice = mipSlice;
			desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE1D;
			break;
		case TEX_TYPE_2D:
			desc.Texture2D.MipSlice = mipSlice;
			desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
			break;
		case TEX_TYPE_3D:
			desc.Texture3D.MipSlice = mipSlice;
			desc.Texture3D.FirstWSlice = firstArraySlice;
			desc.Texture3D.WSize = numArraySlices;
			desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Invalid texture type for this view type.");
		}

		desc.Format = texture->getDXGIFormat();

		ID3D11UnorderedAccessView* uav = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateUnorderedAccessView(texture->getDX11Resource(), &desc, &uav);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create UnorderedAccessView: " + msg);
		}

		return uav;
	}

	ID3D11DepthStencilView* D3D11TextureView::createDSV(D3D11Texture* texture, 
		UINT32 mipSlice, UINT32 firstArraySlice, UINT32 numArraySlices)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		switch(texture->getTextureType())
		{
		case TEX_TYPE_1D:
			desc.Texture1D.MipSlice = mipSlice;
			desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			break;
		case TEX_TYPE_2D:
			desc.Texture2D.MipSlice = mipSlice;
			if(texture->getMultisampleCount() > 0)
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			else
				desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			break;
		default:
			BS_EXCEPT(InvalidParametersException, "Invalid texture type for this view type.");
		}

		desc.Format = texture->getDXGIFormat();

		ID3D11DepthStencilView* dsv = nullptr;

		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());
		HRESULT hr = d3d11rs->getPrimaryDevice().getD3D11Device()->CreateDepthStencilView(texture->getDX11Resource(), &desc, &dsv);

		if (FAILED(hr) || d3d11rs->getPrimaryDevice().hasError())
		{
			String msg = d3d11rs->getPrimaryDevice().getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Cannot create DepthStencilView: " + msg);
		}

		return dsv;
	}
}
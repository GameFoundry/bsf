//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D11MultiRenderTexture.h"
#include "BsD3D11Texture.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11TextureView.h"

namespace BansheeEngine
{
	D3D11MultiRenderTextureCore::D3D11MultiRenderTextureCore(const MULTI_RENDER_TEXTURE_CORE_DESC& desc)
		:MultiRenderTextureCore(desc), mProperties(desc)
	{

	}

	D3D11MultiRenderTextureCore::~D3D11MultiRenderTextureCore()
	{

	}

	void D3D11MultiRenderTextureCore::getCustomAttribute(const String& name, void* pData) const
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** rtvs = (ID3D11RenderTargetView **)pData;
			for(UINT32 i = 0; i < (UINT32)mColorSurfaces.size(); ++i)		
			{
				if (mColorSurfaces[i] == nullptr)
					continue;

				D3D11TextureView* textureView = static_cast<D3D11TextureView*>(mColorSurfaces[i].get());
				rtvs[i] = textureView->getRTV();	
			}
		}
		else if(name == "DSV")
		{
			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView **)pData;

			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(mDepthStencilSurface.get());
			*dsv = depthStencilView->getDSV(false);
		}
		else if (name == "RODSV")
		{
			ID3D11DepthStencilView** dsv = (ID3D11DepthStencilView **)pData;

			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(mDepthStencilSurface.get());
			*dsv = depthStencilView->getDSV(true);
		}
	}

	D3D11MultiRenderTexture::D3D11MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTexture(desc), mProperties(desc)
	{ }
}
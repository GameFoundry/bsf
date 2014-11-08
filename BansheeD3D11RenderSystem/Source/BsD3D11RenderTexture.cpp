#include "BsD3D11RenderTexture.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsD3D11Texture.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11TextureView.h"
#include "BsTextureManager.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D11RenderTextureCore::D3D11RenderTextureCore(D3D11RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
		const RENDER_SURFACE_DESC& depthStencilSurfaceDesc)
		:RenderTextureCore(parent, properties, colorSurfaceDesc, depthStencilSurfaceDesc)
	{ }

	void D3D11RenderTextureCore::getCustomAttribute(const String& name, void* pData) const
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** pRTVs = (ID3D11RenderTargetView **)pData;			
			D3D11TextureView* textureView = static_cast<D3D11TextureView*>(mColorSurface.get());
			*pRTVs = textureView->getRTV();		

			return;
		}
		else if(name == "DSV")
		{
			ID3D11DepthStencilView** pDSV = (ID3D11DepthStencilView **)pData;
			D3D11TextureView* depthStencilView = static_cast<D3D11TextureView*>(mDepthStencilSurface.get());
			*pDSV = depthStencilView->getDSV();
			return;
		}
	}

	RenderTargetProperties* D3D11RenderTexture::createProperties() const
	{
		return bs_new<RenderTextureProperties>();
	}

	SPtr<CoreObjectCore> D3D11RenderTexture::createCore() const
	{
		RenderTextureProperties* coreProperties = bs_new<RenderTextureProperties>();
		RenderTextureProperties* myProperties = static_cast<RenderTextureProperties*>(mProperties);

		*coreProperties = *myProperties;

		return bs_shared_ptr<D3D11RenderTextureCore>(const_cast<D3D11RenderTexture*>(this), 
			coreProperties, mColorSurfaceDesc, mDepthStencilSurfaceDesc);
	}
}
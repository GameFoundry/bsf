#include "BsD3D11MultiRenderTexture.h"
#include "BsD3D11Texture.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11TextureView.h"

namespace BansheeEngine
{
	D3D11MultiRenderTextureCore::D3D11MultiRenderTextureCore(D3D11MultiRenderTexture* parent, 
		MultiRenderTextureProperties* properties, const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTextureCore(parent, properties, desc)
	{

	}

	D3D11MultiRenderTextureCore::~D3D11MultiRenderTextureCore()
	{

	}

	void D3D11MultiRenderTextureCore::getCustomAttribute(const String& name, void* pData) const
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** pRTVs = (ID3D11RenderTargetView **)pData;
			for(size_t x = 0; x < mColorSurfaces.size(); ++x)		
			{
				D3D11TextureView* textureView = static_cast<D3D11TextureView*>(mColorSurfaces[x].get());
				pRTVs[x] = textureView->getRTV();	
			}

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

	RenderTargetProperties* D3D11MultiRenderTexture::createProperties() const
	{
		return bs_new<MultiRenderTextureProperties>();
	}

	SPtr<CoreObjectCore> D3D11MultiRenderTexture::createCore() const
	{
		MultiRenderTextureProperties* coreProperties = bs_new<MultiRenderTextureProperties>();
		MultiRenderTextureProperties* myProperties = static_cast<MultiRenderTextureProperties*>(mProperties);

		*coreProperties = *myProperties;

		return bs_shared_ptr<D3D11MultiRenderTextureCore>(const_cast<D3D11MultiRenderTexture*>(this),
			coreProperties, mDesc);
	}
}
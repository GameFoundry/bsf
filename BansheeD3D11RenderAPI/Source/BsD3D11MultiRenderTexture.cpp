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

	D3D11MultiRenderTexture::D3D11MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTexture(desc), mProperties(desc)
	{ }
}
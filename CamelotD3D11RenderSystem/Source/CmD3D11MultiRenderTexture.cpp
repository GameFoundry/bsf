#include "CmD3D11MultiRenderTexture.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"
#include "CmD3D11TextureView.h"

namespace CamelotEngine
{
	D3D11MultiRenderTexture::D3D11MultiRenderTexture()
		:MultiRenderTexture()
	{

	}

	D3D11MultiRenderTexture::~D3D11MultiRenderTexture()
	{

	}

	void D3D11MultiRenderTexture::getCustomAttribute(const String& name, void* pData)
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
}
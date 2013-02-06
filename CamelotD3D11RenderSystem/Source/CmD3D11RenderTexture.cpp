#include "CmD3D11RenderTexture.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11Texture.h"
#include "CmD3D11Mappings.h"
#include "CmD3D11TextureView.h"
#include "CmTextureManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11RenderTexture::D3D11RenderTexture()
		:RenderTexture()
	{

	}

	D3D11RenderTexture::~D3D11RenderTexture()
	{
	}

	void D3D11RenderTexture::initialize_internal()
	{
		// Do nothing
	}

	void D3D11RenderTexture::getCustomAttribute(const String& name, void* pData)
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
}
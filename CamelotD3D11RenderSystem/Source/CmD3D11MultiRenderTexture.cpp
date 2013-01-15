#include "CmD3D11MultiRenderTexture.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"

namespace CamelotEngine
{
	D3D11MultiRenderTexture::D3D11MultiRenderTexture()
		:MultiRenderTexture(), mDepthStencilView(nullptr)
	{

	}

	D3D11MultiRenderTexture::~D3D11MultiRenderTexture()
	{

	}

	void D3D11MultiRenderTexture::setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(mRenderTargetViews.size() <= surfaceIdx)
			mRenderTargetViews.resize(surfaceIdx + 1, nullptr);

		SAFE_RELEASE(mRenderTargetViews[surfaceIdx]);

		if(texture != nullptr)
		{
			mRenderTargetViews[surfaceIdx] = D3D11RenderTexture::createRenderTargetView(mSurfaces[surfaceIdx]);
		}
	}

	void D3D11MultiRenderTexture::setDepthStencilImpl(TexturePtr depthStencilSurface)
	{
		SAFE_RELEASE(mDepthStencilView);

		if(mDepthStencilView != nullptr)
		{
			mDepthStencilView = D3D11RenderTexture::createDepthStencilView(mDepthStencilSurface);
		}
	}

	void D3D11MultiRenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name == "RTV")
		{
			ID3D11RenderTargetView** pRTVs = (ID3D11RenderTargetView **)pData;
			for(size_t x = 0; x < mRenderTargetViews.size(); ++x)							
				pRTVs[x] = mRenderTargetViews[x];			

			return;
		}
		else if(name == "DSV")
		{
			ID3D11DepthStencilView** pDSV = (ID3D11DepthStencilView **)pData;

			*pDSV = mDepthStencilView;
			return;
		}
	}

	void D3D11MultiRenderTexture::initialize()
	{
		mRenderTargetViews.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}
}
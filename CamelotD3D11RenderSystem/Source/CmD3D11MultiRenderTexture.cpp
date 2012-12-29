#include "CmD3D11MultiRenderTexture.h"
#include "CmD3D11DepthStencilBuffer.h"
#include "CmD3D11Texture.h"
#include "CmD3D11RenderTexture.h"

namespace CamelotEngine
{
	D3D11MultiRenderTexture::D3D11MultiRenderTexture()
		:MultiRenderTexture()
	{

	}

	D3D11MultiRenderTexture::~D3D11MultiRenderTexture()
	{

	}

	void D3D11MultiRenderTexture::setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		SAFE_RELEASE(mRenderTargetViews[surfaceIdx]);

		if(texture != nullptr)
		{
			mRenderTargetViews[surfaceIdx] = D3D11RenderTexture::createRenderTargetView(mSurfaces[surfaceIdx]);
		}
	}

	void D3D11MultiRenderTexture::setDepthStencilImpl(DepthStencilBufferPtr depthStencilBuffer)
	{

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
			D3D11DepthStencilBuffer* d3d11depthStencilBuffer = static_cast<D3D11DepthStencilBuffer*>(mDepthStencilBuffer.get());

			*pDSV = d3d11depthStencilBuffer->getDepthStencilView();
			return;
		}
	}

	void D3D11MultiRenderTexture::initialize()
	{
		mRenderTargetViews.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}
}
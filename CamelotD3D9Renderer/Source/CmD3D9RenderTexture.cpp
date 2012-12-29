#include "CmD3D9RenderTexture.h"
#include "CmD3D9Texture.h"
#include "CmD3D9HardwarePixelBuffer.h"
#include "CmD3D9RenderSystem.h"
#include "CmD3D9DepthStencilBuffer.h"

namespace CamelotEngine
{
	D3D9RenderTexture::D3D9RenderTexture()
		:mColorSurface(nullptr), mDepthStencilSurface(nullptr)
	{

	}

	D3D9RenderTexture::~D3D9RenderTexture()
	{

	}

	void D3D9RenderTexture::getCustomAttribute_internal(const String& name, void* pData)
	{
		if(name == "DDBACKBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
			*pSurf = mColorSurface;
			return;
		}
		else if(name == "D3DZBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
			*pSurf = mDepthStencilSurface;
			return;
		}
		else if(name == "HWND")
		{
			HWND *pHwnd = (HWND*)pData;
			*pHwnd = NULL;
			return;
		}
	}

	void D3D9RenderTexture::createInternalResourcesImpl()
	{
		D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(mTexture.get());
		D3D9HardwarePixelBuffer* pixelBuffer = static_cast<D3D9HardwarePixelBuffer*>(d3d9texture->getBuffer(mFace, mMipLevel).get());
		mColorSurface = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());

		D3D9DepthStencilBuffer* d3d9DepthStencil = static_cast<D3D9DepthStencilBuffer*>(mDepthStencilBuffer.get());
		mDepthStencilSurface = d3d9DepthStencil->getSurface();
	}
}
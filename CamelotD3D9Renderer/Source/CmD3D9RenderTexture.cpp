#include "CmD3D9RenderTexture.h"
#include "CmD3D9Texture.h"
#include "CmD3D9PixelBuffer.h"
#include "CmD3D9RenderSystem.h"
#include "CmTextureView.h"

namespace BansheeEngine
{
	D3D9RenderTexture::D3D9RenderTexture()
		:mDX9ColorSurface(nullptr), mDX9DepthStencilSurface(nullptr), mIsBindableToShader(false)
	{

	}

	D3D9RenderTexture::~D3D9RenderTexture()
	{

	}

	void D3D9RenderTexture::getCustomAttribute(const String& name, void* pData) const
	{
		if(name == "DDBACKBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
			*pSurf = mDX9ColorSurface;
			return;
		}
		else if(name == "D3DZBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;
			*pSurf = mDX9DepthStencilSurface;
			return;
		}
		else if(name == "HWND")
		{
			HWND *pHwnd = (HWND*)pData;
			*pHwnd = NULL;
			return;
		}
	}

	void D3D9RenderTexture::initialize_internal()
	{
		D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(mColorSurface->getTexture().get());
		D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(
			d3d9texture->getBuffer(mColorSurface->getFirstArraySlice(), mColorSurface->getMostDetailedMip()).get());
		mDX9ColorSurface = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());

		D3D9Texture* d3d9DepthStencil = static_cast<D3D9Texture*>(mDepthStencilSurface->getTexture().get());
		D3D9PixelBuffer* depthStencilBuffer = static_cast<D3D9PixelBuffer*>(
			d3d9DepthStencil->getBuffer(mDepthStencilSurface->getFirstArraySlice(), mDepthStencilSurface->getMostDetailedMip()).get());
		mDX9DepthStencilSurface = depthStencilBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());

		RenderTexture::initialize_internal();
	}
}
#include "CmD3D9MultiRenderTexture.h"
#include "CmD3D9DepthStencilBuffer.h"
#include "CmD3D9Texture.h"
#include "CmD3D9RenderSystem.h"

namespace CamelotEngine
{
	D3D9MultiRenderTexture::D3D9MultiRenderTexture()
		:MultiRenderTexture(), mDepthStencilSurface(nullptr)
	{

	}

	D3D9MultiRenderTexture::~D3D9MultiRenderTexture()
	{

	}

	void D3D9MultiRenderTexture::setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(texture != nullptr)
		{
			D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(texture.get());
			D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(d3d9texture->getBuffer(face, mipLevel).get());
			mColorSurfaces[surfaceIdx] = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
		}
		else
		{
			mColorSurfaces[surfaceIdx] = nullptr;
		}
	}

	void D3D9MultiRenderTexture::setDepthStencilImpl(DepthStencilBufferPtr depthStencilBuffer)
	{
		if(depthStencilBuffer != nullptr)
		{
			D3D9DepthStencilBuffer* d3d9DepthStencil = static_cast<D3D9DepthStencilBuffer*>(mDepthStencilBuffer.get());
			mDepthStencilSurface = d3d9DepthStencil->getSurface();
		}
		else
		{
			mDepthStencilSurface = nullptr;
		}
	}

	void D3D9MultiRenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name == "DDBACKBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;

			for(size_t x = 0; x < mColorSurfaces.size(); ++x)							
				pSurf[x] = mColorSurfaces[x];			

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

	void D3D9MultiRenderTexture::initialize()
	{
		mColorSurfaces.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}
}
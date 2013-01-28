#include "CmD3D9MultiRenderTexture.h"
#include "CmD3D9Texture.h"
#include "CmD3D9RenderSystem.h"

namespace CamelotEngine
{
	D3D9MultiRenderTexture::D3D9MultiRenderTexture()
		:MultiRenderTexture(), mDX9DepthStencilSurface(nullptr)
	{

	}

	D3D9MultiRenderTexture::~D3D9MultiRenderTexture()
	{

	}

	void D3D9MultiRenderTexture::initialize_internal()
	{
		mDX9ColorSurfaces.resize(CM_MAX_MULTIPLE_RENDER_TARGETS);
	}

	void D3D9MultiRenderTexture::setColorSurfaceImpl(UINT32 surfaceIdx, TexturePtr texture, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(texture != nullptr)
		{
			D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(texture.get());
			D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(d3d9texture->getBuffer(face, mipLevel).get());
			mDX9ColorSurfaces[surfaceIdx] = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
		}
		else
		{
			mDX9ColorSurfaces[surfaceIdx] = nullptr;
		}
	}

	void D3D9MultiRenderTexture::setDepthStencilImpl(TexturePtr depthStencilSurface, UINT32 face, UINT32 numFaces, UINT32 mipLevel)
	{
		if(depthStencilSurface != nullptr)
		{
			D3D9Texture* d3d9DepthStencil = static_cast<D3D9Texture*>(depthStencilSurface.get());
			D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(d3d9DepthStencil->getBuffer(face, mipLevel).get());
			mDX9DepthStencilSurface = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
		}
		else
		{
			mDX9DepthStencilSurface = nullptr;
		}
	}

	void D3D9MultiRenderTexture::getCustomAttribute(const String& name, void* pData)
	{
		if(name == "DDBACKBUFFER")
		{
			IDirect3DSurface9 ** pSurf = (IDirect3DSurface9 **)pData;

			for(size_t x = 0; x < mDX9ColorSurfaces.size(); ++x)							
				pSurf[x] = mDX9ColorSurfaces[x];			

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
}
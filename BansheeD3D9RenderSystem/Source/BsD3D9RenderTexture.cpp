#include "BsD3D9RenderTexture.h"
#include "BsD3D9Texture.h"
#include "BsD3D9PixelBuffer.h"
#include "BsD3D9RenderSystem.h"
#include "BsTextureView.h"

namespace BansheeEngine
{
	D3D9RenderTextureCore::D3D9RenderTextureCore(D3D9RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
		const RENDER_SURFACE_DESC& depthStencilSurfaceDesc)
		:RenderTextureCore(parent, properties, colorSurfaceDesc, depthStencilSurfaceDesc), mDX9ColorSurface(nullptr), 
		mDX9DepthStencilSurface(nullptr), mIsBindableToShader(false)
	{ }

	D3D9RenderTextureCore::~D3D9RenderTextureCore()
	{ }

	void D3D9RenderTextureCore::initialize()
	{
		RenderTextureCore::initialize();

		initializeSurfaces();
	}

	void D3D9RenderTextureCore::initializeSurfaces()
	{
		D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(mColorSurface->getTexture().get());
		D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(
			d3d9texture->getBuffer(mColorSurface->getFirstArraySlice(), mColorSurface->getMostDetailedMip()).get());
		mDX9ColorSurface = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());

		D3D9Texture* d3d9DepthStencil = static_cast<D3D9Texture*>(mDepthStencilSurface->getTexture().get());
		D3D9PixelBuffer* depthStencilBuffer = static_cast<D3D9PixelBuffer*>(
			d3d9DepthStencil->getBuffer(mDepthStencilSurface->getFirstArraySlice(), mDepthStencilSurface->getMostDetailedMip()).get());
		mDX9DepthStencilSurface = depthStencilBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
	}

	void D3D9RenderTextureCore::releaseSurfaces()
	{
		// All actual releasing happens in the color and depth textures.
		mDX9ColorSurface = nullptr;
		mDX9DepthStencilSurface = nullptr;
	}

	void D3D9RenderTextureCore::getCustomAttribute(const String& name, void* pData) const
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

	void D3D9RenderTextureCore::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		initializeSurfaces();
	}

	void D3D9RenderTextureCore::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		releaseSurfaces();
	}

	void D3D9RenderTextureCore::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		releaseSurfaces();
	}

	void D3D9RenderTextureCore::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{
		initializeSurfaces();
	}

	RenderTargetProperties* D3D9RenderTexture::createProperties() const
	{
		return bs_new<RenderTextureProperties>();
	}

	CoreObjectCore* D3D9RenderTexture::createCore() const
	{
		RenderTextureProperties* coreProperties = bs_new<RenderTextureProperties>();
		RenderTextureProperties* myProperties = static_cast<RenderTextureProperties*>(mProperties);

		*coreProperties = *myProperties;

		return bs_new<D3D9RenderTextureCore>(const_cast<D3D9RenderTexture*>(this),
			coreProperties, mColorSurfaceDesc, mDepthStencilSurfaceDesc);
	}
}
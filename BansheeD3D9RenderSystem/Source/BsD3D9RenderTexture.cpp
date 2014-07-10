//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D9RenderTexture.h"
#include "BsD3D9Texture.h"
#include "BsD3D9PixelBuffer.h"
#include "BsD3D9RenderSystem.h"
#include "BsTextureView.h"

namespace BansheeEngine
{
	D3D9RenderTexture::D3D9RenderTexture()
		:mDX9ColorSurface(nullptr), mDX9DepthStencilSurface(nullptr), mIsBindableToShader(false)
	{

	}

	D3D9RenderTexture::~D3D9RenderTexture()
	{

	}

	void D3D9RenderTexture::initialize_internal()
	{
		initializeSurfaces();

		RenderTexture::initialize_internal();
	}

	void D3D9RenderTexture::initializeSurfaces()
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

	void D3D9RenderTexture::releaseSurfaces()
	{
		// All actual releasing happens in the color and depth textures.
		mDX9ColorSurface = nullptr;
		mDX9DepthStencilSurface = nullptr;
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

	void D3D9RenderTexture::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		initializeSurfaces();
	}

	void D3D9RenderTexture::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		releaseSurfaces();
	}

	void D3D9RenderTexture::notifyOnDeviceLost(IDirect3DDevice9* d3d9Device)
	{
		releaseSurfaces();
	}

	void D3D9RenderTexture::notifyOnDeviceReset(IDirect3DDevice9* d3d9Device)
	{
		initializeSurfaces();
	}
}
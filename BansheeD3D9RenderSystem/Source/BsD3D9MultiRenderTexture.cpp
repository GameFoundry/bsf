#include "BsD3D9MultiRenderTexture.h"
#include "BsD3D9Texture.h"
#include "BsD3D9RenderSystem.h"

namespace BansheeEngine
{
	D3D9MultiRenderTextureCore::D3D9MultiRenderTextureCore(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTextureCore(desc), mProperties(desc), mDX9DepthStencilSurface(nullptr)
	{ }

	D3D9MultiRenderTextureCore::~D3D9MultiRenderTextureCore()
	{ }

	void D3D9MultiRenderTextureCore::initialize()
	{
		MultiRenderTextureCore::initialize();

		mDX9ColorSurfaces.resize(mColorSurfaces.size());

		for (size_t i = 0; i < mColorSurfaces.size(); i++)
		{
			if (mColorSurfaces[i] != nullptr)
			{
				D3D9Texture* d3d9texture = static_cast<D3D9Texture*>(mColorSurfaces[i]->getTexture().get());
				if (d3d9texture->getTextureType() != TEX_TYPE_3D)
				{
					D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(
						d3d9texture->getBuffer(mColorSurfaces[i]->getDesc().firstArraySlice, mColorSurfaces[i]->getDesc().mostDetailMip).get());
					mDX9ColorSurfaces[i] = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
				}
				else
					mDX9ColorSurfaces[i] = nullptr;
			}
			else
			{
				mDX9ColorSurfaces[i] = nullptr;
			}
		}

		if (mDepthStencilSurface != nullptr)
		{
			D3D9Texture* d3d9DepthStencil = static_cast<D3D9Texture*>(mDepthStencilSurface->getTexture().get());

			if (d3d9DepthStencil->getTextureType() != TEX_TYPE_3D)
			{
				D3D9PixelBuffer* pixelBuffer = static_cast<D3D9PixelBuffer*>(
					d3d9DepthStencil->getBuffer(mDepthStencilSurface->getDesc().firstArraySlice, mDepthStencilSurface->getDesc().mostDetailMip).get());
				mDX9DepthStencilSurface = pixelBuffer->getSurface(D3D9RenderSystem::getActiveD3D9Device());
			}
			else
				mDX9DepthStencilSurface = nullptr;
		}
		else
		{
			mDX9DepthStencilSurface = nullptr;
		}
	}

	void D3D9MultiRenderTextureCore::getCustomAttribute(const String& name, void* pData) const
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

	D3D9MultiRenderTexture::D3D9MultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
		:MultiRenderTexture(desc), mProperties(desc)
	{

	}
}
#include "BsD3D9TextureManager.h"
#include "BsD3D9Texture.h"
#include "BsD3D9RenderTexture.h"
#include "BsException.h"
#include "BsD3D9Mappings.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9MultiRenderTexture.h"

namespace BansheeEngine 
{
	D3D9TextureManager::D3D9TextureManager()
		:TextureManager()
	{ }
	
	D3D9TextureManager::~D3D9TextureManager()
	{ }

    TexturePtr D3D9TextureManager::createTextureImpl()
    {
		D3D9Texture* tex = new (bs_alloc<D3D9Texture, PoolAlloc>()) D3D9Texture();

		return bs_core_ptr<D3D9Texture, PoolAlloc>(tex);
    }

	RenderTexturePtr D3D9TextureManager::createRenderTextureImpl()
	{
		D3D9RenderTexture* tex = new (bs_alloc<D3D9RenderTexture, PoolAlloc>()) D3D9RenderTexture();

		return bs_core_ptr<D3D9RenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr D3D9TextureManager::createMultiRenderTextureImpl()
	{
		D3D9MultiRenderTexture* tex = new (bs_alloc<D3D9MultiRenderTexture, PoolAlloc>()) D3D9MultiRenderTexture();

		return bs_core_ptr<D3D9MultiRenderTexture, PoolAlloc>(tex);
	}

	PixelFormat D3D9TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		if((usage & TU_RENDERTARGET) != 0)
		{
			return D3D9Mappings::_getClosestSupportedRenderTargetPF(format);
		}
		else if((usage & TU_DEPTHSTENCIL) != 0)
		{
			return D3D9Mappings::_getClosestSupportedDepthStencilPF(format);
		}
		else
		{
			// Basic filtering
			return D3D9Mappings::_getClosestSupportedPF(format);
		}
	}
}

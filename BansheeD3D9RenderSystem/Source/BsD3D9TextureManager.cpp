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

	RenderTexturePtr D3D9TextureManager::createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc)
	{
		D3D9RenderTexture* tex = new (bs_alloc<D3D9RenderTexture, PoolAlloc>()) D3D9RenderTexture(desc);

		return bs_core_ptr<D3D9RenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr D3D9TextureManager::createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		D3D9MultiRenderTexture* tex = new (bs_alloc<D3D9MultiRenderTexture, PoolAlloc>()) D3D9MultiRenderTexture(desc);

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

	SPtr<TextureCore> D3D9TextureCoreManager::createTextureInternal(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
		int numMips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 multisampleCount)
	{
		D3D9TextureCore* tex = new (bs_alloc<D3D9TextureCore>()) D3D9TextureCore(texType,
			width, height, depth, numMips, format, usage, hwGammaCorrection, multisampleCount);

		SPtr<D3D9TextureCore> texPtr = bs_shared_ptr<D3D9TextureCore, GenAlloc>(tex);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}

	SPtr<RenderTextureCore> D3D9TextureCoreManager::createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc)
	{
		SPtr<D3D9RenderTextureCore> texPtr = bs_shared_ptr<D3D9RenderTextureCore>(desc);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}

	SPtr<MultiRenderTextureCore> D3D9TextureCoreManager::createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		SPtr<D3D9MultiRenderTextureCore> texPtr = bs_shared_ptr<D3D9MultiRenderTextureCore>(desc);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}
}

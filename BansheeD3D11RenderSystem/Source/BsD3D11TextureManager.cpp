#include "BsD3D11TextureManager.h"
#include "BsD3D11Texture.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11MultiRenderTexture.h"

namespace BansheeEngine
{
	RenderTexturePtr D3D11TextureManager::createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc)
	{
		D3D11RenderTexture* tex = new (bs_alloc<D3D11RenderTexture, PoolAlloc>()) D3D11RenderTexture(desc);

		return bs_core_ptr<D3D11RenderTexture, PoolAlloc>(tex);
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		D3D11MultiRenderTexture* tex = new (bs_alloc<D3D11MultiRenderTexture, PoolAlloc>()) D3D11MultiRenderTexture(desc);

		return bs_core_ptr<D3D11MultiRenderTexture, PoolAlloc>(tex);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::getPF(D3D11Mappings::getClosestSupportedPF(format, hwGamma), hwGamma);

		return D3D11Mappings::getPF(d3dPF);
	}

	SPtr<TextureCore> D3D11TextureCoreManager::createTextureInternal(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
		int numMips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 multisampleCount)
	{
		D3D11TextureCore* tex = new (bs_alloc<D3D11TextureCore>()) D3D11TextureCore(texType, 
			width, height, depth, numMips, format, usage, hwGammaCorrection, multisampleCount);

		return bs_shared_ptr<D3D11TextureCore, GenAlloc>(tex);
	}

	SPtr<RenderTextureCore> D3D11TextureCoreManager::createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc)
	{
		return bs_shared_ptr<D3D11RenderTextureCore>(desc);
	}

	SPtr<MultiRenderTextureCore> D3D11TextureCoreManager::createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		return bs_shared_ptr<D3D11MultiRenderTextureCore>(desc);
	}
}
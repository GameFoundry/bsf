#include "BsD3D11TextureManager.h"
#include "BsD3D11Texture.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11RenderAPI.h"
#include "BsD3D11MultiRenderTexture.h"

namespace BansheeEngine
{
	RenderTexturePtr D3D11TextureManager::createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc)
	{
		D3D11RenderTexture* tex = new (bs_alloc<D3D11RenderTexture>()) D3D11RenderTexture(desc);

		return bs_core_ptr<D3D11RenderTexture>(tex);
	}

	MultiRenderTexturePtr D3D11TextureManager::createMultiRenderTextureImpl(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		D3D11MultiRenderTexture* tex = new (bs_alloc<D3D11MultiRenderTexture>()) D3D11MultiRenderTexture(desc);

		return bs_core_ptr<D3D11MultiRenderTexture>(tex);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		// Basic filtering
		DXGI_FORMAT d3dPF = D3D11Mappings::getPF(D3D11Mappings::getClosestSupportedPF(format, hwGamma), hwGamma);

		return D3D11Mappings::getPF(d3dPF);
	}

	SPtr<TextureCore> D3D11TextureCoreManager::createTextureInternal(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
		int numMips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 multisampleCount, const PixelDataPtr& initialData)
	{
		D3D11TextureCore* tex = new (bs_alloc<D3D11TextureCore>()) D3D11TextureCore(texType, 
			width, height, depth, numMips, format, usage, hwGammaCorrection, multisampleCount, initialData);

		SPtr<D3D11TextureCore> texPtr = bs_shared_ptr<D3D11TextureCore>(tex);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}

	SPtr<RenderTextureCore> D3D11TextureCoreManager::createRenderTextureInternal(const RENDER_TEXTURE_CORE_DESC& desc)
	{
		SPtr<D3D11RenderTextureCore> texPtr = bs_shared_ptr_new<D3D11RenderTextureCore>(desc);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}

	SPtr<MultiRenderTextureCore> D3D11TextureCoreManager::createMultiRenderTextureInternal(const MULTI_RENDER_TEXTURE_CORE_DESC& desc)
	{
		SPtr<D3D11MultiRenderTextureCore> texPtr = bs_shared_ptr_new<D3D11MultiRenderTextureCore>(desc);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}
}
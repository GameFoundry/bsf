//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsD3D11TextureManager.h"
#include "BsD3D11Texture.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11Mappings.h"
#include "BsD3D11RenderAPI.h"

namespace bs
{
	SPtr<RenderTexture> D3D11TextureManager::createRenderTextureImpl(const RENDER_TEXTURE_DESC& desc)
	{
		D3D11RenderTexture* tex = new (bs_alloc<D3D11RenderTexture>()) D3D11RenderTexture(desc);

		return bs_core_ptr<D3D11RenderTexture>(tex);
	}

	PixelFormat D3D11TextureManager::getNativeFormat(TextureType ttype, PixelFormat format, int usage, bool hwGamma)
	{
		DXGI_FORMAT d3dPF = ct::D3D11Mappings::getPF(
			ct::D3D11Mappings::getClosestSupportedPF(format, ttype, usage), hwGamma);

		return ct::D3D11Mappings::getPF(d3dPF);
	}

	namespace ct
	{
	SPtr<Texture> D3D11TextureManager::createTextureInternal(const TEXTURE_DESC& desc,
		const SPtr<PixelData>& initialData, GpuDeviceFlags deviceMask)
	{
		D3D11Texture* tex = new (bs_alloc<D3D11Texture>()) D3D11Texture(desc, initialData, deviceMask);

		SPtr<D3D11Texture> texPtr = bs_shared_ptr<D3D11Texture>(tex);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}

	SPtr<RenderTexture> D3D11TextureManager::createRenderTextureInternal(const RENDER_TEXTURE_DESC& desc,
																				 UINT32 deviceIdx)
	{
		SPtr<D3D11RenderTexture> texPtr = bs_shared_ptr_new<D3D11RenderTexture>(desc, deviceIdx);
		texPtr->_setThisPtr(texPtr);

		return texPtr;
	}
	}
}

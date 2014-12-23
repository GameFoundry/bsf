#include "BsTextureManager.h"
#include "BsException.h"
#include "BsPixelUtil.h"
#include "BsMultiRenderTexture.h"
#include "BsRenderAPI.h"

namespace BansheeEngine 
{
    TexturePtr TextureManager::createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, int numMipmaps,
        PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount)
    {
		Texture* tex = new (bs_alloc<Texture>()) Texture(texType, width, height, depth, numMipmaps, format, usage, hwGamma, multisampleCount);
		TexturePtr ret = bs_core_ptr<Texture, GenAlloc>(tex);

		ret->_setThisPtr(ret);
		ret->initialize();

		return ret;
    }

	TexturePtr TextureManager::_createEmpty()
	{
		Texture* tex = new (bs_alloc<Texture>()) Texture();
		TexturePtr texture = bs_core_ptr<Texture, GenAlloc>(tex);
		texture->_setThisPtr(texture);

		return texture;
	}

	RenderTexturePtr TextureManager::createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format, bool hwGamma, UINT32 multisampleCount, 
			bool createDepth, PixelFormat depthStencilFormat)
	{
		HTexture texture = Texture::create(textureType, width, height, 0, format, TU_RENDERTARGET, hwGamma, multisampleCount);

		HTexture depthStencil;
		if(createDepth)
		{
			depthStencil = Texture::create(TEX_TYPE_2D, width, height, 0, depthStencilFormat, TU_DEPTHSTENCIL, false, multisampleCount);
		}

		RENDER_TEXTURE_DESC desc;
		desc.colorSurface.texture = texture;
		desc.colorSurface.face = 0;
		desc.colorSurface.mipLevel = 0;

		desc.depthStencilSurface.texture = depthStencil;
		desc.depthStencilSurface.face = 0;
		desc.depthStencilSurface.mipLevel = 0;

		RenderTexturePtr newRT = createRenderTexture(desc);

		return newRT;
	}

	RenderTexturePtr TextureManager::createRenderTexture(const RENDER_TEXTURE_DESC& desc)
	{
		RenderTexturePtr newRT = createRenderTextureImpl(desc);
		newRT->_setThisPtr(newRT);
		newRT->initialize();

		return newRT;
	}

	MultiRenderTexturePtr TextureManager::createMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		MultiRenderTexturePtr newRT = createMultiRenderTextureImpl(desc);
		newRT->_setThisPtr(newRT);
		newRT->initialize();

		return newRT;
	}

	SPtr<TextureCore> TextureCoreManager::createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth,
		int numMips, PixelFormat format, int usage, bool hwGammaCorrection, UINT32 multisampleCount)
	{
		SPtr<TextureCore> newRT = createTextureInternal(texType, width, height, depth, numMips, format, usage, hwGammaCorrection, multisampleCount);
		newRT->initialize();

		return newRT;
	}

	SPtr<RenderTextureCore> TextureCoreManager::createRenderTexture(const RENDER_TEXTURE_CORE_DESC& desc)
	{
		SPtr<RenderTextureCore> newRT = createRenderTextureInternal(desc);
		newRT->initialize();

		return newRT;
	}

	SPtr<MultiRenderTextureCore> TextureCoreManager::createMultiRenderTexture(const MULTI_RENDER_TEXTURE_CORE_DESC& desc)
	{
		SPtr<MultiRenderTextureCore> newRT = createMultiRenderTextureInternal(desc);
		newRT->initialize();

		return newRT;
	}
}

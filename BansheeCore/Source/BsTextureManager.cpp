#include "BsTextureManager.h"
#include "BsException.h"
#include "BsPixelUtil.h"
#include "BsMultiRenderTexture.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
    TextureManager::TextureManager()
    {
        // Subclasses should register (when this is fully constructed)
    }

    TextureManager::~TextureManager()
    {
        // subclasses should unregister with resource group manager
    }

	void TextureManager::onStartUp()
	{
		// Internally this will call our createTextureImpl virtual method. But since this is guaranteed
		// to be the last class in the hierarchy, we can call a virtual method from constructor.
		mDummyTexture = Texture::create(TEX_TYPE_2D, 2, 2, 0, PF_R8G8B8A8);

		UINT32 subresourceIdx = mDummyTexture->mapToSubresourceIdx(0, 0);
		PixelDataPtr data = mDummyTexture->allocateSubresourceBuffer(subresourceIdx);

		data->setColorAt(Color::Red, 0, 0);
		data->setColorAt(Color::Red, 0, 1);
		data->setColorAt(Color::Red, 1, 0);
		data->setColorAt(Color::Red, 1, 1);

		AsyncOp op;

		data->_lock();
		RenderSystem::instance().writeSubresource(mDummyTexture.getInternalPtr(), mDummyTexture->mapToSubresourceIdx(0, 0), data, false, op);
	}

    TexturePtr TextureManager::createTexture(TextureType texType, UINT32 width, UINT32 height, UINT32 depth, int numMipmaps,
        PixelFormat format, int usage, bool hwGamma, UINT32 multisampleCount)
    {
        TexturePtr ret = createTextureImpl();
		ret->_setThisPtr(ret);
		ret->initialize(texType, width, height, depth, static_cast<size_t>(numMipmaps), format, usage, hwGamma, multisampleCount);

		return ret;
    }

	TexturePtr TextureManager::_createEmpty()
	{
		TexturePtr texture = createTextureImpl();
		texture->_setThisPtr(texture);

		return texture;
	}

	RenderTexturePtr TextureManager::createRenderTexture(TextureType textureType, UINT32 width, UINT32 height, 
			PixelFormat format, bool hwGamma, UINT32 multisampleCount, 
			bool createDepth, PixelFormat depthStencilFormat)
	{
		TexturePtr texture = createTexture(textureType, width, height, 0, format, TU_RENDERTARGET, hwGamma, multisampleCount);

		TexturePtr depthStencil = nullptr;
		if(createDepth)
		{
			depthStencil = createTexture(TEX_TYPE_2D, width, height, 0, depthStencilFormat, TU_DEPTHSTENCIL, false, multisampleCount);
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
		RenderTexturePtr newRT = createRenderTextureImpl();
		newRT->_setThisPtr(newRT);
		newRT->initialize(desc);

		return newRT;
	}

	MultiRenderTexturePtr TextureManager::createMultiRenderTexture(const MULTI_RENDER_TEXTURE_DESC& desc)
	{
		MultiRenderTexturePtr newRT = createMultiRenderTextureImpl();
		newRT->_setThisPtr(newRT);
		newRT->initialize(desc);

		return newRT;
	}
}

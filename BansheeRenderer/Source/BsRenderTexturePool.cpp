#include "BsRenderTexturePool.h"
#include "BsRenderTexture.h"
#include "BsTexture.h"
#include "BsTextureManager.h"

namespace BansheeEngine
{
	PooledRenderTexture::PooledRenderTexture(RenderTexturePool* pool)
		:mIsFree(false), mPool(pool)
	{ }

	PooledRenderTexture::~PooledRenderTexture()
	{
		if (mPool != nullptr)
			mPool->_unregisterTexture(this);
	}

	RenderTexturePool::~RenderTexturePool()
	{
		for (auto& texture : mTextures)
			texture.second->mPool = nullptr;
	}

	SPtr<PooledRenderTexture> RenderTexturePool::get(PixelFormat format, UINT32 width, UINT32 height, bool hwGamma, UINT32 samples)
	{
		bool depth = PixelUtil::isDepth(format);

		for (auto& texturePair : mTextures)
		{
			SPtr<PooledRenderTexture> textureData = texturePair.second;

			if (!textureData->mIsFree)
				continue;

			SPtr<TextureCore> textureCore;
			if (!depth)
				textureCore = textureData->texture->getBindableColorTexture(); 
			else
				textureCore = textureData->texture->getBindableDepthStencilTexture();

			if (textureCore == nullptr)
				continue;

			if (matches(textureCore, format, width, height, hwGamma, samples))
			{
				textureData->mIsFree = false;
				return textureData;
			}
		}

		SPtr<PooledRenderTexture> newTextureData = bs_shared_ptr<PooledRenderTexture>(this);
		_registerTexture(newTextureData);

		RENDER_SURFACE_CORE_DESC surfaceDesc;
		surfaceDesc.texture = TextureCoreManager::instance().createTexture(TEX_TYPE_2D, width, height, 1, 0, 
			format, depth ? TU_DEPTHSTENCIL : TU_RENDERTARGET, hwGamma, samples);
		surfaceDesc.face = 0;
		surfaceDesc.mipLevel = 0;

		RENDER_TEXTURE_CORE_DESC desc;
		if (!depth)
			desc.colorSurface = surfaceDesc;
		else
			desc.depthStencilSurface = surfaceDesc;

		newTextureData->texture = TextureCoreManager::instance().createRenderTexture(desc);
		return newTextureData;
	}

	void RenderTexturePool::free(const SPtr<PooledRenderTexture>& texture)
	{
		auto iterFind = mTextures.find(texture.get());
		iterFind->second->mIsFree = true;
	}

	bool RenderTexturePool::matches(const SPtr<TextureCore>& texture, PixelFormat format, UINT32 width, UINT32 height, bool hwGamma, UINT32 samples)
	{
		const TextureProperties& texProps = texture->getProperties();
		return texProps.getFormat() == format && texProps.getWidth() == width && texProps.getHeight() == height &&
			texProps.isHardwareGammaEnabled() == hwGamma && texProps.getMultisampleCount() == samples;
	}

	void RenderTexturePool::_registerTexture(const SPtr<PooledRenderTexture>& texture)
	{
		mTextures.insert(std::make_pair(texture.get(), texture));
	}

	void RenderTexturePool::_unregisterTexture(PooledRenderTexture* texture)
	{
		mTextures.erase(texture);
	}
}
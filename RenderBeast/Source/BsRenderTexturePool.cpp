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
			texture.second.lock()->mPool = nullptr;
	}

	SPtr<PooledRenderTexture> RenderTexturePool::get(PixelFormat format, UINT32 width, UINT32 height, bool hwGamma, UINT32 samples)
	{
		bool depth = PixelUtil::isDepth(format);

		for (auto& texturePair : mTextures)
		{
			SPtr<PooledRenderTexture> textureData = texturePair.second.lock();

			if (!textureData->mIsFree)
				continue;

			if (textureData->texture == nullptr)
				continue;

			if (matches(textureData->texture, format, width, height, hwGamma, samples))
			{
				textureData->mIsFree = false;
				return textureData;
			}
		}

		SPtr<PooledRenderTexture> newTextureData = bs_shared_ptr_new<PooledRenderTexture>(this);
		_registerTexture(newTextureData);

		newTextureData->texture = TextureCoreManager::instance().createTexture(TEX_TYPE_2D, width, height, 1, 0,
			format, depth ? TU_DEPTHSTENCIL : TU_RENDERTARGET, hwGamma, samples);
		
		return newTextureData;
	}

	void RenderTexturePool::release(const SPtr<PooledRenderTexture>& texture)
	{
		auto iterFind = mTextures.find(texture.get());
		iterFind->second.lock()->mIsFree = true;
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
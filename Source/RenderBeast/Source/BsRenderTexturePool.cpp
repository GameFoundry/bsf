//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

	SPtr<PooledRenderTexture> RenderTexturePool::get(const POOLED_RENDER_TEXTURE_DESC& desc)
	{
		for (auto& texturePair : mTextures)
		{
			SPtr<PooledRenderTexture> textureData = texturePair.second.lock();

			if (!textureData->mIsFree)
				continue;

			if (textureData->texture == nullptr)
				continue;

			if (matches(textureData->texture, desc))
			{
				textureData->mIsFree = false;
				return textureData;
			}
		}

		SPtr<PooledRenderTexture> newTextureData = bs_shared_ptr_new<PooledRenderTexture>(this);
		_registerTexture(newTextureData);

		newTextureData->texture = TextureCoreManager::instance().createTexture(desc.type, desc.width, desc.height, 
			desc.depth, 0, desc.format, desc.flag, desc.hwGamma, desc.numSamples);
		
		return newTextureData;
	}

	void RenderTexturePool::release(const SPtr<PooledRenderTexture>& texture)
	{
		auto iterFind = mTextures.find(texture.get());
		iterFind->second.lock()->mIsFree = true;
	}

	bool RenderTexturePool::matches(const SPtr<TextureCore>& texture, const POOLED_RENDER_TEXTURE_DESC& desc)
	{
		const TextureProperties& texProps = texture->getProperties();

		bool match = texProps.getTextureType() == desc.type && texProps.getFormat() == desc.format && 
			texProps.getWidth() == desc.width && texProps.getHeight() == desc.height && texProps.getUsage() == desc.flag;

		if (!match)
			return false;

		if(desc.type == TEX_TYPE_2D)
			return texProps.isHardwareGammaEnabled() == desc.hwGamma && texProps.getMultisampleCount() == desc.numSamples;

		if(desc.type == TEX_TYPE_3D)
			return texProps.getDepth() == desc.depth;

		if (desc.type == TEX_TYPE_CUBE_MAP)
			return true;

		return false;
	}

	void RenderTexturePool::_registerTexture(const SPtr<PooledRenderTexture>& texture)
	{
		mTextures.insert(std::make_pair(texture.get(), texture));
	}

	void RenderTexturePool::_unregisterTexture(PooledRenderTexture* texture)
	{
		mTextures.erase(texture);
	}

	POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::create2D(PixelFormat format, UINT32 width, UINT32 height,
		INT32 usage, UINT32 samples, bool hwGamma)
	{
		POOLED_RENDER_TEXTURE_DESC desc;
		desc.width = width;
		desc.height = height;
		desc.depth = 1;
		desc.format = format;
		desc.numSamples = samples;
		desc.flag = (TextureUsage)usage;
		desc.hwGamma = hwGamma;
		desc.type = TEX_TYPE_2D;

		return desc;
	}

	POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::create3D(PixelFormat format, UINT32 width, UINT32 height, 
		UINT32 depth, INT32 usage)
	{
		POOLED_RENDER_TEXTURE_DESC desc;
		desc.width = width;
		desc.height = height;
		desc.depth = depth;
		desc.format = format;
		desc.numSamples = 1;
		desc.flag = (TextureUsage)usage;
		desc.hwGamma = false;
		desc.type = TEX_TYPE_3D;

		return desc;
	}

	POOLED_RENDER_TEXTURE_DESC POOLED_RENDER_TEXTURE_DESC::createCube(PixelFormat format, UINT32 width, UINT32 height,
		INT32 usage)
	{
		POOLED_RENDER_TEXTURE_DESC desc;
		desc.width = width;
		desc.height = height;
		desc.depth = 1;
		desc.format = format;
		desc.numSamples = 1;
		desc.flag = (TextureUsage)usage;
		desc.hwGamma = false;
		desc.type = TEX_TYPE_CUBE_MAP;

		return desc;
	}
}
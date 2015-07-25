#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	class RenderTexturePool;

	struct PooledRenderTexture
	{
		PooledRenderTexture(RenderTexturePool* pool);
		~PooledRenderTexture();

		SPtr<RenderTextureCore> texture;

	private:
		friend class RenderTexturePool;

		RenderTexturePool* mPool;
		bool mIsFree;
	};

	class RenderTexturePool
	{
	public:
		~RenderTexturePool();

		SPtr<PooledRenderTexture> get(PixelFormat format, UINT32 width, UINT32 height, bool hwGamma = false, UINT32 samples = 0);
		void free(const SPtr<PooledRenderTexture>& texture);

	private:
		friend struct PooledRenderTexture;

		void _registerTexture(const SPtr<PooledRenderTexture>& texture);
		void _unregisterTexture(PooledRenderTexture* texture);

		bool matches(const SPtr<TextureCore>& texture, PixelFormat format, UINT32 width, UINT32 height, bool hwGamma, UINT32 samples);

		Map<PooledRenderTexture*, SPtr<PooledRenderTexture>> mTextures;
	};
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsModule.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	class RenderTexturePool;

	/**	Contains data about a single render texture in the texture pool. */
	struct PooledRenderTexture
	{
		PooledRenderTexture(RenderTexturePool* pool);
		~PooledRenderTexture();

		SPtr<TextureCore> texture;

	private:
		friend class RenderTexturePool;

		RenderTexturePool* mPool;
		bool mIsFree;
	};

	/** Contains a pool of render textures meant to accommodate reuse of render textures of the same size and format. */
	class RenderTexturePool : public Module<RenderTexturePool>
	{
	public:
		~RenderTexturePool();

		/**
		 * Attempts to find the unused render texture with the specified parameters in the pool, or creates a new texture
		 * otherwise. When done with the texture make sure to call release().
		 *
		 * @param[in]	format		Pixel format used by the texture color surface.
		 * @param[in]	width		Width of the render texture, in pixels.
		 * @param[in]	height		Height of the render texture, in pixels.
		 * @param[in]	hwGamma		Should the written pixels be gamma corrected.
		 * @param[in]	samples		If higher than 1, texture containing multiple samples per pixel is created.
		 */
		SPtr<PooledRenderTexture> get(PixelFormat format, UINT32 width, UINT32 height, bool hwGamma = false, 
			UINT32 samples = 0);

		/**
		 * Releases a texture previously allocated with get(). The texture is returned to the pool so that it may be reused
		 * later.
		 *			
		 * @note	
		 * The texture will be removed from the pool if the last reference to it is deleted. Normally you would call 
		 * release() but keep a reference if you plan on using it later on.
		 */
		void release(const SPtr<PooledRenderTexture>& texture);

	private:
		friend struct PooledRenderTexture;

		/**	Registers a newly created render texture in the pool. */
		void _registerTexture(const SPtr<PooledRenderTexture>& texture);

		/**	Unregisters a created render texture in the pool. */
		void _unregisterTexture(PooledRenderTexture* texture);

		/**
		 * Checks does the provided texture match the parameters.
		 * 
		 * @param[in]	texture		Texture to match against the parameters.
		 * @param[in]	format		Pixel format used by the texture color surface.
		 * @param[in]	width		Width of the render texture, in pixels.
		 * @param[in]	height		Height of the render texture, in pixels.
		 * @param[in]	hwGamma		Should the written pixels be gamma corrected.
		 * @param[in]	samples		If higher than 1, texture containing multiple samples per pixel is created.
		 */
		bool matches(const SPtr<TextureCore>& texture, PixelFormat format, UINT32 width, UINT32 height, bool hwGamma,
			UINT32 samples);

		Map<PooledRenderTexture*, std::weak_ptr<PooledRenderTexture>> mTextures;
	};

	/** @} */
}
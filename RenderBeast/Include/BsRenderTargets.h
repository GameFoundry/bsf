//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Allocates and handles all the required render targets
	 *			for rendering a scene from a specific viewport.
	 *
	 * @note	Core thread only.
	 */
	class BS_BSRND_EXPORT RenderTargets
	{
	public:
		/**
		 * @brief	Creates a new set of render targets. This will not actually allocate 
		 *			the internal render targets - this happens the first time you call ::bind.
		 *
		 * @param	viewport		Viewport that the render targets will be used for. Determines size of the
		 *							render targets, and the output color render target.
		 * @param	hdr				Should the render targets support high dynamic range rendering.
		 * @param	numSamples		Number of samples to use if multisampling is active. Provide 0 or 1 if
		 *							multisampled targets are not needed.
		 */
		static SPtr<RenderTargets> create(const SPtr<ViewportCore>& viewport, bool hdr, UINT32 numSamples);

		/**
		 * @brief	Allocates the textures required for rendering. Allocations are pooled so this is generally a fast 
		 * 			operation unless the size or other render target options changed. This must be called before binding
		 * 			render targets.
		 */
		void allocate();

		/**
		 * @brief	Deallocates textures by returning them to the pool. This should be done when the caller is done using 
		 * 			the render targets, so that other systems might re-use them. This will not release any memory unless
		 * 			all render targets pointing to those textures go out of scope.
		 */
		void release();

		/**
		 * @brief	Binds the GBuffer render target for rendering.
		 */
		void bindGBuffer();

		/**
		 * @brief	Binds the scene color render target for rendering.
		 */
		void bindSceneColor(bool readOnlyDepthStencil);

		/**
		 * @brief	Resolves the GBuffer scene color into the output scene color buffer.
		 */
		void resolve();

		/**
		 * @brief	Returns the first color texture of the gbuffer as a bindable texture.
		 */
		SPtr<TextureCore> getTextureA() const;

		/**
		* @brief	Returns the second color texture of the gbuffer as a bindable texture.
		*/
		SPtr<TextureCore> getTextureB() const;

		/**
		 * @brief	Returns the depth texture of the gbuffer as a bindable texture.
		 */
		SPtr<TextureCore> getTextureDepth() const;

		/**
		 * @brief	Checks if the targets support HDR rendering.
		 */
		bool getHDR() const { return mHDR; }

		/**
		 * @brief	Returns the number of samples per pixel supported by the targets.
		 */
		UINT32 getNumSamples() const { return mNumSamples; }

	private:
		RenderTargets(const SPtr<ViewportCore>& viewport, bool hdr, UINT32 numSamples);

		/**
		 * @brief	Returns the width of gbuffer textures, in pixels.
		 */
		UINT32 getWidth() const;

		/**
		 * @brief	Returns the height of gbuffer textures, in pixels.
		 */
		UINT32 getHeight() const;

		SPtr<ViewportCore> mViewport;

		SPtr<PooledRenderTexture> mSceneColorTex;
		SPtr<PooledRenderTexture> mAlbedoTex;
		SPtr<PooledRenderTexture> mNormalTex;
		SPtr<PooledRenderTexture> mDepthTex;

		SPtr<MultiRenderTextureCore> mGBufferRT;
		SPtr<RenderTextureCore> mSceneColorRT;

		PixelFormat mDiffuseFormat;
		PixelFormat mNormalFormat;
		UINT32 mNumSamples;
		bool mHDR;
	};
}
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
		 *							render targets.
		 * @param	hdr				Should the render targets support high dynamic range rendering.
		 * @param	numSamples		Number of samples to use if multisampling is active. Provide 0 or 1 if
		 *							multisampled targets are not needed.
		 */
		static SPtr<RenderTargets> create(const ViewportCore& viewport, bool hdr, UINT32 numSamples);

		/**
		 * @brief	Binds the render targets for rendering. This will also allocate the render
		 *			targets if they aren't already allocated.
		 */
		void bind();

		/**
		 * @brief	Frees the render targets so they may be used by another set of render targets. This
		 *			will not release the render target memory. Memory will only released once all
		 *			RenderTarget instances pointing to the render target go out of scope.
		 */
		void unbind();

	private:
		RenderTargets(const ViewportCore& viewport, bool hdr, UINT32 numSamples);

		SPtr<PooledRenderTexture> mDiffuseRT;
		SPtr<PooledRenderTexture> mNormalRT;
		SPtr<PooledRenderTexture> mDepthRT;

		SPtr<MultiRenderTextureCore> mGBuffer;

		UINT32 mWidth;
		UINT32 mHeight;
		PixelFormat mDiffuseFormat;
		PixelFormat mNormalFormat;
		UINT32 mNumSamples;
	};
}
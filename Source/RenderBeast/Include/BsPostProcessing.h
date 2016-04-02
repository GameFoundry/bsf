//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace BansheeEngine
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	/** Settings that control the post-process operation. */
	struct PostProcessSettings
	{
		/** 
		 * Determines minimum luminance value in the eye adaptation histogram. In log2 units (-8 = 1/256). In the range
		 * [-16, 0]. 
		 */
		float histogramLog2Min = -8.0f; 

		/** 
		 * Determines maximum luminance value in the eye adaptation histogram. In log2 units (4 = 16). In the range
		 * [0, 16]. 
		 */
		float histogramLog2Max = 4.0f;
	};

	BS_PARAM_BLOCK_BEGIN(DownsampleParams)
		BS_PARAM_BLOCK_ENTRY(Vector2, gInvTexSize)
	BS_PARAM_BLOCK_END

	/** Shader that downsamples a texture to half its size. */
	class DownsampleMat : public RendererMaterial<DownsampleMat>
	{
		RMAT_DEF("PPDownsample.bsl");

	public:
		DownsampleMat();

		/** Updates the parameter buffers used by the material. */
		void setParameters(const SPtr<RenderTextureCore>& target);
	private:
		DownsampleParams mParams;
		MaterialParamVec2Core mInvTexSize;
		MaterialParamTextureCore mInputTexture;
	};

	BS_PARAM_BLOCK_BEGIN(EyeAdaptHistogramParams)
		BS_PARAM_BLOCK_ENTRY(Vector4I, gPixelOffsetAndSize)
		BS_PARAM_BLOCK_ENTRY(Vector2, gHistogramParams)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gThreadGroupCount)
	BS_PARAM_BLOCK_END

	/** Shader that creates a luminance histogram used for eye adaptation. */
	class EyeAdaptHistogramMat : public RendererMaterial<EyeAdaptHistogramMat>
	{
		RMAT_DEF("PPEyeAdaptHistogram.bsl");

	public:
		EyeAdaptHistogramMat();

		/** Updates the parameter buffers used by the material. */
		void setParameters(const SPtr<RenderTextureCore>& target, const PostProcessSettings& settings);
	private:
		EyeAdaptHistogramParams mParams;
		MaterialParamTextureCore mSceneColor;
		MaterialParamLoadStoreTextureCore mOutputTex;

		static const INT32 THREAD_GROUP_SIZE_X = 4;
		static const INT32 THREAD_GROUP_SIZE_Y = 4;
		static const INT32 LOOP_COUNT_X = 8;
		static const INT32 LOOP_COUNT_Y = 8;
	};

	/**
	 * Renders post-processing effects for the provided render target.
	 *
	 * @note	Core thread only.
	 */
	class BS_BSRND_EXPORT PostProcessing
	{
	public:
		/** Renders post-processing effects for the provided render target. */
		static void postProcess(const SPtr<RenderTextureCore>& target, const PostProcessSettings& settings);
		
	private:
	};

	/** @} */
}
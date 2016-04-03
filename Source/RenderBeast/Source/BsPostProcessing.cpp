//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPostProcessing.h"
#include "BsRenderTexture.h"
#include "BsRenderTexturePool.h"
#include "BsRendererUtility.h"

namespace BansheeEngine
{
	DownsampleMat::DownsampleMat()
	{
		mMaterial->setParamBlockBuffer("Input", mParams.getBuffer());

		mInputTexture = mMaterial->getParamTexture("gInputTex");
		mInvTexSize = mMaterial->getParamVec2("gInvTexSize");
	}

	void DownsampleMat::_initDefines(ShaderDefines& defines)
	{
		// Do nothing
	}

	void DownsampleMat::execute(const SPtr<RenderTextureCore>& target, PostProcessInfo& ppInfo)
	{
		// Set parameters
		SPtr<TextureCore> colorTexture = target->getBindableColorTexture();
		mInputTexture.set(colorTexture);

		const RenderTextureProperties& rtProps = target->getProperties();
		Vector2 invTextureSize(1.0f / rtProps.getWidth(), 1.0f / rtProps.getHeight());

		mParams.gInvTexSize.set(invTextureSize);

		// Set output
		const TextureProperties& colorProps = colorTexture->getProperties();

		UINT32 width = std::max(1, Math::ceilToInt(colorProps.getWidth() * 0.5f));
		UINT32 height = std::max(1, Math::ceilToInt(colorProps.getHeight() * 0.5f));

		mOutputDesc = POOLED_RENDER_TEXTURE_DESC::create2D(colorProps.getFormat(), width, height, TU_RENDERTARGET);

		// Render
		ppInfo.downsampledSceneTex = RenderTexturePool::instance().get(mOutputDesc);

		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.setRenderTarget(ppInfo.downsampledSceneTex->renderTexture, true);

		gRendererUtility().setPass(mMaterial, 0);
		gRendererUtility().drawScreenQuad();

		mOutput = ppInfo.downsampledSceneTex->renderTexture;
	}

	void DownsampleMat::release(PostProcessInfo& ppInfo)
	{
		RenderTexturePool::instance().release(ppInfo.downsampledSceneTex);
		mOutput = nullptr;
	}

	EyeAdaptHistogramMat::EyeAdaptHistogramMat()
	{
		mMaterial->setParamBlockBuffer("Input", mParams.getBuffer());

		mSceneColor = mMaterial->getParamTexture("gSceneColorTex");
		mOutputTex = mMaterial->getParamLoadStoreTexture("gOutputTex");
	}

	void EyeAdaptHistogramMat::_initDefines(ShaderDefines& defines)
	{
		defines.set("THREADGROUP_SIZE_X", THREAD_GROUP_SIZE_X);
		defines.set("THREADGROUP_SIZE_Y", THREAD_GROUP_SIZE_Y);
		defines.set("LOOP_COUNT_X", LOOP_COUNT_X);
		defines.set("LOOP_COUNT_Y", LOOP_COUNT_Y);
	}

	void EyeAdaptHistogramMat::execute(const SPtr<RenderTextureCore>& target, PostProcessInfo& ppInfo)
	{
		// Set parameters
		mSceneColor.set(target->getBindableColorTexture());

		const PostProcessSettings& settings = ppInfo.settings;

		float diff = settings.histogramLog2Max - settings.histogramLog2Min;
		float scale = 1.0f / diff;
		float offset = -settings.histogramLog2Min * scale;

		const RenderTextureProperties& props = target->getProperties();
		int offsetAndSize[4] = { 0, 0, (INT32)props.getWidth(), (INT32)props.getHeight() };

		mParams.gHistogramParams.set(Vector2(scale, offset));
		mParams.gPixelOffsetAndSize.set(Vector4I(offsetAndSize));

		const UINT32 texelsPerThreadGroupX = THREAD_GROUP_SIZE_X * LOOP_COUNT_X;
		const UINT32 texelsPerThreadGroupY = THREAD_GROUP_SIZE_Y * LOOP_COUNT_Y;

		Vector2I threadGroupCount;
		threadGroupCount.x = ((INT32)props.getWidth() + texelsPerThreadGroupX - 1) / texelsPerThreadGroupX;
		threadGroupCount.y = ((INT32)props.getHeight() + texelsPerThreadGroupY - 1) / texelsPerThreadGroupY;

		mParams.gThreadGroupCount.set(threadGroupCount);

		// Set output
		UINT32 histogramSize = THREAD_GROUP_SIZE_X * THREAD_GROUP_SIZE_Y;
		UINT32 histogramTexelCount = histogramSize / 4; // 4 entries per texel
		UINT32 numHistograms = threadGroupCount.x * threadGroupCount.y;

		mOutputDesc = POOLED_RENDER_TEXTURE_DESC::create2D(PF_FLOAT32_RGBA, histogramTexelCount, numHistograms, 
			TU_LOADSTORE);

		// Dispatch
		ppInfo.histogramTex = RenderTexturePool::instance().get(mOutputDesc);

		mOutputTex.set(ppInfo.histogramTex->texture);

		// TODO - Clear downsampled scene texture as render target before dispatch?
		RenderAPICore& rapi = RenderAPICore::instance();
		rapi.dispatchCompute(threadGroupCount.x, threadGroupCount.y);

		mOutput = ppInfo.histogramTex->renderTexture;
	}

	void EyeAdaptHistogramMat::release(PostProcessInfo& ppInfo)
	{
		RenderTexturePool::instance().release(ppInfo.histogramTex);
		mOutput = nullptr;
	}

	void PostProcessing::postProcess(const SPtr<RenderTextureCore>& target, PostProcessInfo& ppInfo)
	{
		mDownsample.execute(target, ppInfo);
		mEyeAdaptHistogram.execute(mDownsample.getOutput(), ppInfo);

		mDownsample.release(ppInfo);
		mEyeAdaptHistogram.release(ppInfo);

		// TODO - Use plain white for initial eye adaptation tex needed for histogram reduce
	}
}
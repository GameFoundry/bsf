//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "Renderer/BsRendererMaterial.h"
#include "Renderer/BsParamBlocks.h"
#include "RenderAPI/BsGpuPipelineParamInfo.h"
#include "BsRendererLight.h"
#include "BsRendererReflectionProbe.h"

namespace bs { namespace ct
{
	struct SkyInfo;
	struct SceneInfo;
	class RendererViewGroup;

	/** @addtogroup RenderBeast
	 *  @{
	 */

	BS_PARAM_BLOCK_BEGIN(TiledLightingParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector4I, gLightCounts)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gLightStrides)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gFramebufferSize)
	BS_PARAM_BLOCK_END

	extern TiledLightingParamDef gTiledLightingParamDef;

	/** Shader that performs a lighting pass over data stored in the Gbuffer. */
	class TiledDeferredLightingMat : public RendererMaterial<TiledDeferredLightingMat>
	{
		RMAT_DEF_CUSTOMIZED("TiledDeferredLighting.bsl");

		/** Helper method used for initializing variations of this material. */
		template<UINT32 msaa>
		static const ShaderVariation& getVariation()
		{
			static ShaderVariation variation = ShaderVariation(
			Vector<ShaderVariation::Param>{
				ShaderVariation::Param("MSAA_COUNT", msaa)
			});

			return variation;
		}
	public:
		TiledDeferredLightingMat();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const RendererView& view, const VisibleLightData& lightData, const GBufferTextures& gbuffer,
			const SPtr<Texture>& lightAccumTex, const SPtr<GpuBuffer>& lightAccumBuffer, const SPtr<Texture>& msaaCoverage);

		/** Returns the material variation matching the provided parameters. */
		static TiledDeferredLightingMat* getVariation(UINT32 msaaCount);

	private:
		UINT32 mSampleCount;
		GBufferParams mGBufferParams;

		GpuParamBuffer mLightBufferParam;
		GpuParamLoadStoreTexture mOutputTextureParam;
		GpuParamBuffer mOutputBufferParam;
		GpuParamTexture mMSAACoverageTexParam;

		SPtr<GpuParamBlockBuffer> mParamBuffer;

		static const UINT32 TILE_SIZE;
	};

	BS_PARAM_BLOCK_BEGIN(FlatFramebufferToTextureParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gFramebufferSize)
		BS_PARAM_BLOCK_ENTRY(INT32, gSampleCount)
	BS_PARAM_BLOCK_END

	/** Shader that copies a flattened framebuffer into a multisampled texture. */
	class FlatFramebufferToTextureMat : public RendererMaterial<FlatFramebufferToTextureMat>
	{
		RMAT_DEF("FlatFramebufferToTexture.bsl");

	public:
		FlatFramebufferToTextureMat();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const SPtr<GpuBuffer>& flatFramebuffer, const SPtr<Texture>& target);
	private:
		GpuParamBuffer mInputParam;
		SPtr<GpuParamBlockBuffer> mParamBuffer;
	};

	BS_PARAM_BLOCK_BEGIN(TiledImageBasedLightingParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gFramebufferSize)
	BS_PARAM_BLOCK_END

	extern TiledImageBasedLightingParamDef gTiledImageBasedLightingParamDef;

	/** Shader that performs a lighting pass over data stored in the Gbuffer. */
	class TiledDeferredImageBasedLightingMat : public RendererMaterial<TiledDeferredImageBasedLightingMat>
	{
		RMAT_DEF_CUSTOMIZED("TiledDeferredImageBasedLighting.bsl");

		/** Helper method used for initializing variations of this material. */
		template<UINT32 msaa>
		static const ShaderVariation& getVariation()
		{
			static ShaderVariation variation = ShaderVariation(
			Vector<ShaderVariation::Param>{
				ShaderVariation::Param("MSAA_COUNT", msaa)
			});

			return variation;
		}
	public:
		/** Container for parameters to be passed to the execute() method. */
		struct Inputs
		{
			GBufferTextures gbuffer;
			SPtr<Texture> lightAccumulation;
			SPtr<Texture> sceneColorTex;
			SPtr<GpuBuffer> sceneColorBuffer;
			SPtr<Texture> preIntegratedGF;
			SPtr<Texture> ambientOcclusion;
			SPtr<Texture> ssr;
			SPtr<Texture> msaaCoverage;
		};

		TiledDeferredImageBasedLightingMat();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const RendererView& view, const SceneInfo& sceneInfo, const VisibleReflProbeData& probeData, 
			const Inputs& inputs);

		/** Returns the material variation matching the provided parameters. */
		static TiledDeferredImageBasedLightingMat* getVariation(UINT32 msaaCount);

	private:
		UINT32 mSampleCount;

		GpuParamTexture mGBufferA;
		GpuParamTexture mGBufferB;
		GpuParamTexture mGBufferC;
		GpuParamTexture mGBufferDepth;

		GpuParamTexture mInColorTextureParam;
		GpuParamTexture mMSAACoverageTexParam;

		ImageBasedLightingParams mImageBasedParams;

		GpuParamLoadStoreTexture mOutputTextureParam;
		GpuParamBuffer mOutputBufferParam;

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		ReflProbeParamBuffer mReflProbeParamBuffer;

		static const UINT32 TILE_SIZE;
	};

	/** @} */
}}

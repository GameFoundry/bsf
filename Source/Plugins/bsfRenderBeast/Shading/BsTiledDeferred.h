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
			{
				ShaderVariation::Param("MSAA_COUNT", msaa)
			});

			return variation;
		}
	public:
		TiledDeferredLightingMat();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const RendererView& view, const VisibleLightData& lightData, const GBufferTextures& gbuffer,
			const SPtr<Texture>& inputTexture, const SPtr<Texture>& lightAccumTex, const SPtr<Texture>& lightAccumTexArray,
			const SPtr<Texture>& msaaCoverage);

		/** Returns the material variation matching the provided parameters. */
		static TiledDeferredLightingMat* getVariation(UINT32 msaaCount);

	private:
		UINT32 mSampleCount;
		GBufferParams mGBufferParams;

		GpuParamBuffer mLightBufferParam;
		GpuParamLoadStoreTexture mOutputTextureParam;

		GpuParamTexture mInColorTextureParam;
		GpuParamTexture mMSAACoverageTexParam;

		SPtr<GpuParamBlockBuffer> mParamBuffer;

		static const UINT32 TILE_SIZE;
	};

	/**
	 * Moves data from a texture array into a MSAA texture. Primarily useful when needing to do unordered writes to a
	 * MSAA texture which isn't directly supported on some backends, so writes are done to a texture array instead. The
	 * array is expected to have the same number of layers as the number of samples in the MSAA texture, each layer
	 * containing a sample for that specific pixel.
	 */
	class TextureArrayToMSAATexture : public RendererMaterial<TextureArrayToMSAATexture>
	{
		RMAT_DEF("TextureArrayToMSAATexture.bsl");

	public:
		TextureArrayToMSAATexture();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const SPtr<Texture>& inputArray, const SPtr<Texture>& target);

	private:
		GpuParamTexture mInputParam;
	};

	BS_PARAM_BLOCK_BEGIN(ClearLoadStoreParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector2I, gSize)
		BS_PARAM_BLOCK_ENTRY(Vector4, gFloatClearVal)
		BS_PARAM_BLOCK_ENTRY(Vector4I, gIntClearVal)
	BS_PARAM_BLOCK_END

	extern ClearLoadStoreParamDef gClearLoadStoreParamDef;

	/** Possible object types used as clear destinations by ClearLoadStoreMat. */
	enum class ClearLoadStoreType { Texture, TextureArray, Buffer, StructuredBuffer };

	/** Possible data types used in destination objects in ClearLoadStoreMat. */
	enum class ClearLoadStoreDataType { Float, Int };

	/** Clears the provided texture to zero, using a compute shader. */
	class ClearLoadStoreMat : public RendererMaterial<ClearLoadStoreMat>
	{
		RMAT_DEF_CUSTOMIZED("ClearLoadStore.bsl");

	public:
		ClearLoadStoreMat();

		/**
		 * Binds the material for rendering, sets up parameters and executes it. Only works on variations of
		 * this material intended for textures and texture arrays.
		 */
		void execute(const SPtr<Texture>& target, const Color& clearValue = Color::ZERO,
				const TextureSurface& surface = TextureSurface::COMPLETE);

		/**
		 * Binds the material for rendering, sets up parameters and executes it. Only works on variations of
		 * this material intended for buffers.
		 */
		void execute(const SPtr<GpuBuffer>& target, const Color& clearValue = Color::ZERO);

		/**
		 * Returns the material variation matching the provided parameters.
		 *
		 * @param[in]		objType			Type of object used for clear source.
		 * @param[in]		dataType		Base data type stored in the clear source object.
		 * @param[in]		numComponents	Number of components in the source objects's data type (e.g. float2, float4).
		 * 									In range [1, 4].
		 * @return							Material variation matching the provided values.
		 */
		static ClearLoadStoreMat* getVariation(ClearLoadStoreType objType, ClearLoadStoreDataType dataType,
				UINT32 numComponents);
	private:
		/** TILE_SIZE * TILE_SIZE is the number of pixels to process per thread. */
		static constexpr UINT32 TILE_SIZE = 4;

		/** Number of threads to launch per work group. */
		static constexpr UINT32 NUM_THREADS = 128;

		GpuParamLoadStoreTexture mOutputTextureParam;
		GpuParamBuffer mOutputBufferParam;
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
			{
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
			SPtr<Texture> sceneColorTexArray;
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

		SPtr<GpuParamBlockBuffer> mParamBuffer;
		ReflProbeParamBuffer mReflProbeParamBuffer;

		static const UINT32 TILE_SIZE;
	};

	/** @} */
}}

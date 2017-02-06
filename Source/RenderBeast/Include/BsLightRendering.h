//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererMaterial.h"
#include "BsParamBlocks.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	/** Information about a single light, as seen by the lighting shader. */
	struct LightData
	{
		Vector3 position;
		float radius;
		Vector3 direction;
		float intensity;
		Vector3 spotAngles;
		float radiusSqrdInv;
		Vector3 color;
	};

	/**	Renderer information specific to a single light. */
	class RendererLight
	{
	public:
		RendererLight(Light* light);

		/** Populates the structure with light parameters. */
		void getParameters(LightData& output) const;

		/** Gets the internal light representation. */
		Light* getInternal() const { return mInternal; }

	private:
		Light* mInternal;
	};

	/** Contains GPU buffers used by the renderer to manipulate lights. */
	class GPULightData
	{
	public:
		GPULightData();

		/** Updates the internal buffers with a new set of lights. */
		void setLights(const Vector<LightData>& lightData, UINT32 numDirLights, UINT32 numRadialLights,
					   UINT32 numSpotLights);

		/** Returns a GPU bindable buffer containing information about every light. */
		SPtr<GpuBuffer> getLightBuffer() const { return mLightBuffer; }

		/** Returns a GPU bindable param buffer containing meta-data about light in the ligth buffer. */
		SPtr<GpuParamBlockBuffer> getParamBuffer() const { return mParamBuffer; }

	private:
		SPtr<GpuParamBlockBuffer> mParamBuffer;
		SPtr<GpuBuffer> mLightBuffer;
	};

	BS_PARAM_BLOCK_BEGIN(TiledLightingParamDef)
		BS_PARAM_BLOCK_ENTRY(Vector3I, gLightOffsets)
	BS_PARAM_BLOCK_END

	extern TiledLightingParamDef gTiledLightingParamDef;

	/** Shader that performs a lighting pass over data stored in the Gbuffer. */
	class TiledDeferredLightingMat : public RendererMaterial<TiledDeferredLightingMat>
	{
		RMAT_DEF("TiledDeferredLighting.bsl");

	public:
		TiledDeferredLightingMat();

		/** Binds the material for rendering, sets up parameters and executes it. */
		void execute(const SPtr<RenderTargets>& gbuffer, const SPtr<GpuParamBlockBuffer>& perCamera);

		/** Binds all the active lights. */
		void setLights(const GPULightData& lightData);
	private:
		GpuParamTexture mGBufferA;
		GpuParamTexture mGBufferB;
		GpuParamTexture mGBufferDepth;

		GpuParamBuffer mLightBufferParam;
		GpuParamLoadStoreTexture mOutputParam;

		static const UINT32 TILE_SIZE;
	};

	/** @} */
}}
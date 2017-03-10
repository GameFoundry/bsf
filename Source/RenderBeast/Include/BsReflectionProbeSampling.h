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

	/** Information about a single reflection probe, as seen by the lighting shader. */
	struct ReflProbeData
	{
		Vector3 position;
		float radius;
		Vector3 boxExtents;
		Matrix4 invBoxTransform;
		float transitionDistance;
		UINT32 cubemapIdx;
		UINT32 type;
	};

	/** Contains GPU buffers used by the renderer to manipulate reflection probes. */
	class GPUReflProbeData
	{
	public:
		GPUReflProbeData();

		/** Updates the internal buffers with a new set of probes. */
		void setProbes(const Vector<ReflProbeData>& probeData, UINT32 numProbes);

		/** Returns a GPU bindable buffer containing information about every reflection probe. */
		SPtr<GpuBuffer> getProbeBuffer() const { return mProbeBuffer; }

		/** Returns the number of reflection probes in the probe buffer. */
		UINT32 getNumProbes() const { return mNumProbes; }

	private:
		SPtr<GpuBuffer> mProbeBuffer;

		UINT32 mNumProbes;
	};

	BS_PARAM_BLOCK_BEGIN(ReflProbeParamsParamDef)
		BS_PARAM_BLOCK_ENTRY(INT32, gReflCubemapNumMips)
		BS_PARAM_BLOCK_ENTRY(INT32, gNumProbes)
		BS_PARAM_BLOCK_ENTRY(INT32, gSkyCubemapAvailable)
		BS_PARAM_BLOCK_ENTRY(INT32, gSkyCubemapNumMips)
	BS_PARAM_BLOCK_END

	extern ReflProbeParamsParamDef gReflProbeParamsParamDef;

	/**	Renderer information specific to a single reflection probe. */
	class RendererReflectionProbe
	{
	public:
		RendererReflectionProbe(ReflectionProbe* probe);

		/** Populates the structure with reflection probe parameters. */
		void getParameters(ReflProbeData& output) const;

		ReflectionProbe* probe;
		UINT32 arrayIdx;
		SPtr<Texture> texture;
		bool customTexture : 1;
		bool textureDirty : 1;
		bool arrayDirty : 1;
		bool errorFlagged : 1;
	};

	/** @} */
}}
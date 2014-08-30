#pragma once

#include "BsPrerequisites.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	/**
	 * @brief	Container for data about a GUI element material.
	 */
	struct GUIMaterialInfo
	{
		HMaterial material;
		GpuParamMat4 worldTransform;
		GpuParamFloat invViewportWidth;
		GpuParamFloat invViewportHeight;
		GpuParamSampState mainTexSampler;
		GpuParamTexture mainTexture;
		GpuParamVec4 tint;
	};
}
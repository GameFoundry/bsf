#pragma once

#include "BsPrerequisites.h"
#include "BsMaterialParam.h"

namespace BansheeEngine
{
	/**
	 * @brief	Container for data about a GUI element material.
	 */
	struct GUIMaterialInfo
	{
		HMaterial material;
		MaterialParamMat4 worldTransform;
		MaterialParamFloat invViewportWidth;
		MaterialParamFloat invViewportHeight;
		MaterialParamSampState mainTexSampler;
		MaterialParamTexture mainTexture;
		MaterialParamVec4 tint;
	};
}
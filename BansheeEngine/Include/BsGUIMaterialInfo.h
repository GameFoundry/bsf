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
		MaterialParamSampState mainTexSampler;
		MaterialParamTexture mainTexture;
		MaterialParamVec4 tint;
	};
}
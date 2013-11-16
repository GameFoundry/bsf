#pragma once

#include "BsPrerequisites.h"
#include "CmGpuParam.h"

namespace BansheeEngine
{
	struct GUIMaterialInfo
	{
		CM::HMaterial material;
		CM::GpuParamMat4 worldTransform;
		CM::GpuParamFloat invViewportWidth;
		CM::GpuParamFloat invViewportHeight;
		CM::GpuParamTexture mainTexture;
	};
}
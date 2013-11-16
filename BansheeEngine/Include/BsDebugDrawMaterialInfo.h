#pragma once

#include "BsPrerequisites.h"
#include "CmGpuParam.h"

namespace BansheeEngine
{
	struct DebugDraw2DClipSpaceMatInfo
	{
		CM::HMaterial material;
	};

	struct DebugDraw2DScreenSpaceMatInfo
	{
		CM::HMaterial material;
		CM::GpuParamFloat invViewportWidth;
		CM::GpuParamFloat invViewportHeight;
	};

	struct DebugDraw3DMatInfo
	{
		CM::HMaterial material;
		CM::GpuParamMat4 matViewProj;
	};
}
#pragma once

#include "BsPrerequisites.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	struct DebugDraw2DClipSpaceMatInfo
	{
		HMaterial material;
	};

	struct DebugDraw2DScreenSpaceMatInfo
	{
		HMaterial material;
		GpuParamFloat invViewportWidth;
		GpuParamFloat invViewportHeight;
	};

	struct DebugDraw3DMatInfo
	{
		HMaterial material;
		GpuParamMat4 matViewProj;
	};
}
//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		GpuParamTexture mainTexture;
		GpuParamVec4 tint;
	};
}
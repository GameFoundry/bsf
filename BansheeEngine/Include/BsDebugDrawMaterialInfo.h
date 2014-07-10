//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisites.h"
#include "BsGpuParam.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains data required for setting up and rendering with 
	 *			material used for debug 2D clip space object rendering.
	 */
	struct DebugDraw2DClipSpaceMatInfo
	{
		HMaterial material;
	};

	/**
	 * @brief	Contains data required for setting up and rendering with 
	 *			material used for debug 2D screen space object rendering.
	 */
	struct DebugDraw2DScreenSpaceMatInfo
	{
		HMaterial material;
		GpuParamFloat invViewportWidth;
		GpuParamFloat invViewportHeight;
	};

	/**
	 * @brief	Contains data required for setting up and rendering with 
	 *			material used for debug 3D object rendering.
	 */
	struct DebugDraw3DMatInfo
	{
		HMaterial material;
		GpuParamMat4 matViewProj;
	};
}
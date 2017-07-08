//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsTriangulation.h"
#include "BsMatrix4.h"

namespace bs { namespace ct
{
	/** @addtogroup RenderBeast
	 *  @{
	 */

	/** Handles any pre-processing for light (irradiance) probe lighting. */
	class LightProbes
	{
		struct LightTetrahedron
		{
			Tetrahedron volume;
			Matrix4 transform;
		};

		// TODO - This should accept LightProbe types as input probably
		void updateProbes(const Vector<Vector3>& positions);
	};

	/** @} */
}}
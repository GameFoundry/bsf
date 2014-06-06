#pragma once

#include "BsCorePrerequisites.h"
#include "BsMaterialProxy.h"
#include "BsDrawOps.h"
#include "BsVector3.h"

namespace BansheeEngine 
{
	/**
	 * @brief	A render operation describing everything we need for rendering
	 * 			a single object.
	 */
	struct BS_CORE_EXPORT RenderOperation
	{
		RenderOperation()
		{ }

		MaterialProxy material;
		MeshRenderData* mesh;
		Vector3 worldPosition;
	};
}
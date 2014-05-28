#pragma once

#include "CmPrerequisites.h"
#include "CmDrawOps.h"
#include "CmVector3.h"

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

		MaterialPtr material;
		MeshBasePtr mesh;
		UINT32 submeshIdx;
		Vector3 worldPosition;
	};
}
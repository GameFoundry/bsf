#pragma once

#include "BsCorePrerequisites.h"
#include "BsMaterial.h"
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

		Material::CoreProxyPtr material;
		MeshRenderDataPtr mesh;
		Vector3 worldPosition;
	};
}
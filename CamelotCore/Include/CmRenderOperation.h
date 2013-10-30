#pragma once

#include "CmPrerequisites.h"
#include "CmDrawOps.h"
#include "CmVector3.h"

namespace CamelotFramework 
{
	struct CM_EXPORT RenderOperation
	{
		RenderOperation()
		{ }

		HMaterial material;
		HMesh mesh;
		UINT32 submeshIdx;
		Vector3 worldPosition;
	};
}
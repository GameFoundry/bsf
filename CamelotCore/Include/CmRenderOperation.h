#pragma once

#include "CmPrerequisites.h"
#include "CmRenderOpMesh.h"
#include "CmVector3.h"

namespace CamelotFramework 
{
	struct CM_EXPORT RenderOperation
	{
		RenderOperation()
		{ }

		HMaterial material;
		RenderOpMesh meshData;
		Vector3 worldPosition;
	};
}
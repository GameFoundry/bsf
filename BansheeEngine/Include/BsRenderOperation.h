#pragma once

#include "BsPrerequisites.h"
#include "CmRenderOpMesh.h"
#include "CmVector3.h"

namespace BansheeEngine 
{
	struct BS_EXPORT RenderOperation
	{
		RenderOperation()
		{ }

		CM::HMaterial material;
		CM::RenderOpMesh meshData;
		CM::Vector3 worldPosition;
	};
}
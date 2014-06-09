#pragma once

#include "BsCorePrerequisites.h"
#include "BsSubMesh.h"
#include "BsBounds.h"

namespace BansheeEngine
{
	/**
	 * Contains part of Mesh data that is used on the core thread for rendering.
	 */
	struct BS_CORE_EXPORT MeshProxy
	{
		MeshPtr mesh;
		SubMesh subMesh;
		Bounds bounds;
	};
}
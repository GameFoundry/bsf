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
		std::weak_ptr<MeshBase> mesh;
		SubMesh subMesh;
		Bounds bounds;
		UINT32 submeshIdx;
	};
}
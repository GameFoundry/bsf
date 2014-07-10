//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
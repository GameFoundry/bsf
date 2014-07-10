//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsDrawOps.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains a data about sub-mesh range and the type of
	 *			primitives contained in the range.
	 */
	struct BS_CORE_EXPORT SubMesh
	{
		SubMesh()
			: indexOffset(0), indexCount(0), drawOp(DOT_TRIANGLE_LIST)
		{ }

		SubMesh(UINT32 indexOffset, UINT32 indexCount, DrawOperationType drawOp):
			indexOffset(indexOffset), indexCount(indexCount), drawOp(drawOp)
		{ }

		UINT32 indexOffset;
		UINT32 indexCount;
		DrawOperationType drawOp;
	};
}
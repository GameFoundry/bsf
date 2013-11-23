#pragma once

#include "CmPrerequisites.h"
#include "CmDrawOps.h"

namespace CamelotFramework
{
	struct CM_EXPORT SubMesh
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
#pragma once

#include "CmPrerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmVertexData.h"
#include "CmIndexData.h"

namespace CamelotFramework 
{
	enum DrawOperationType {
		DOT_POINT_LIST = 1,
		DOT_LINE_LIST = 2,
		DOT_LINE_STRIP = 3,
		DOT_TRIANGLE_LIST = 4,
		DOT_TRIANGLE_STRIP = 5,
		DOT_TRIANGLE_FAN = 6
	};

	struct RenderOpMesh
	{
		RenderOpMesh() 
			:vertexData(nullptr), operationType(DOT_TRIANGLE_LIST), useIndexes(true),
			indexData(nullptr) 
		{ }

		VertexData *vertexData;
		IndexData *indexData;

		DrawOperationType operationType;
		bool useIndexes;
	};
}
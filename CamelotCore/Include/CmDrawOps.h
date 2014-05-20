#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Describes operation that will be used for rendering a certain
	 *			set of vertices.
	 */
	enum DrawOperationType 
	{
		DOT_POINT_LIST = 1,
		DOT_LINE_LIST = 2,
		DOT_LINE_STRIP = 3,
		DOT_TRIANGLE_LIST = 4,
		DOT_TRIANGLE_STRIP = 5,
		DOT_TRIANGLE_FAN = 6
	};

	/**
	* @brief	Converts the number of vertices to number of primitives
	* 			based on the specified draw operation.
	*/
	UINT32 CM_EXPORT vertexCountToPrimCount(DrawOperationType type, UINT32 elementCount);
}
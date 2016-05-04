//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Describes operation that will be used for rendering a certain set of vertices. */
	enum DrawOperationType 
	{
		DOT_POINT_LIST = 1, /**< Each vertex represents a point. */
		DOT_LINE_LIST = 2, /**< Each sequential pair of vertices represent a line. */
		DOT_LINE_STRIP = 3, /**< Each vertex (except the first) forms a line with the previous vertex. */
		DOT_TRIANGLE_LIST = 4, /**< Each sequential 3-tuple of vertices represent a triangle. */
		DOT_TRIANGLE_STRIP = 5, /**< Each vertex (except the first two) form a triangle with the previous two vertices. */
		DOT_TRIANGLE_FAN = 6 /**< Each vertex (except the first two) form a triangle with the first vertex and previous vertex. */
	};

	/** Converts the number of vertices to number of primitives based on the specified draw operation. */
	UINT32 BS_CORE_EXPORT vertexCountToPrimCount(DrawOperationType type, UINT32 elementCount);

	/** @} */
}
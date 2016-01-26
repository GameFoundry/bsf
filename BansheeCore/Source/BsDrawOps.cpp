//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsDrawOps.h"

namespace BansheeEngine
{
	UINT32 vertexCountToPrimCount(DrawOperationType type, UINT32 elementCount)
	{
		UINT32 primCount = 0;
		switch (type)
		{
		case DOT_POINT_LIST:
			primCount = elementCount;
			break;

		case DOT_LINE_LIST:
			primCount = elementCount / 2;
			break;

		case DOT_LINE_STRIP:
			primCount = elementCount - 1;
			break;

		case DOT_TRIANGLE_LIST:
			primCount = elementCount / 3;
			break;

		case DOT_TRIANGLE_STRIP:
			primCount = elementCount - 2;
			break;

		case DOT_TRIANGLE_FAN:
			primCount = elementCount - 2;
			break;
		}

		return primCount;
	}
}
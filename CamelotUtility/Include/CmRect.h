#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmPoint.h"

namespace CamelotFramework
{
	/**
	 * @brief	A rectangle. Although you may use any coordinate system, Camelot assumes
	 * 			that X, Y values represent its bottom left corner, where X increases to the right,
	 * 			and Y increases upwards.
	 */
	class Rect
	{
	public:
		Rect()
			:x(0), y(0), width(0), height(0)
		{ }

		Rect(int _x, int _y, int _width, int _height)
			:x(_x), y(_y), width(_width), height(_height)
		{ }

		bool contains(Point point)
		{
			if(point.x >= x && point.x <= (x + width))
			{
				if(point.y >= y && point.y <= (y + height))
					return true;
			}

			return false;
		}

		int x, y, width, height;
	};
}
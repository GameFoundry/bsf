#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmInt2.h"

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

		bool contains(Int2 point) const
		{
			if(point.x >= x && point.x <= (x + width))
			{
				if(point.y >= y && point.y <= (y + height))
					return true;
			}

			return false;
		}

		void encapsulate(const Rect& other)
		{
			int myRight = x + width;
			int myBottom = y + height;
			int otherRight = other.x + other.width;
			int otherBottom = other.y + other.height;

			if(other.x < x)
				x = other.x;

			if(other.y < y)
				y = other.y;

			if(otherRight > myRight)
				width = otherRight - x;
			else
				width = myRight - x;

			if(otherBottom > myBottom)
				height = otherBottom - y;
			else
				height = myBottom - y;
		}

		int x, y, width, height;
	};
}
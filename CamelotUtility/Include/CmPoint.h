#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	class Point
	{
	public:
		Point()
			:x(0), y(0)
		{ }
		Point(int _x, int _y)
			:x(_x), y(_y)
		{ }

		int x, y;
	};
}
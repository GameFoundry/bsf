#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	struct RectOffset
	{
		RectOffset()
			:left(0), right(0), top(0), bottom(0)
		{ }

		CM::INT32 left, right, top, bottom;
	};
}
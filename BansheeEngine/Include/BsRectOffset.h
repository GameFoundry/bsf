#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Rectangle represented in the form of offsets
	 *			from some parent rectangle.
	 */
	struct RectOffset
	{
		RectOffset()
			:left(0), right(0), top(0), bottom(0)
		{ }

		INT32 left, right, top, bottom;
	};
}
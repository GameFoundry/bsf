//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsPrerequisites.h"

namespace bs
{
	/** @addtogroup Utility-Engine
	 *  @{
	 */

	/** Rectangle represented in the form of offsets from some parent rectangle. */
	struct BS_SCRIPT_EXPORT(pl:true,m:Math) RectOffset
	{
		RectOffset() = default;

		RectOffset(INT32 left, INT32 right, INT32 top, INT32 bottom)
			:left(left), right(right), top(top), bottom(bottom)
		{ }

		bool operator==(const RectOffset& rhs) const
		{
			return left == rhs.left && right == rhs.right && top == rhs.top && bottom == rhs.bottom;
		}

		bool operator!=(const RectOffset& rhs) const
		{
			return !operator==(rhs);
		}

		INT32 left = 0;
		INT32 right = 0;
		INT32 top = 0;
		INT32 bottom = 0;
	};

	/** @} */
}

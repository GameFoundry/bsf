//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** @addtogroup Math
	 *  @{
	 */

	/** Represents a 2D rectangle using integer values. Rectangle is represented with an origin in top left and width/height. */
	class BS_UTILITY_EXPORT Rect2I
	{
	public:
		constexpr Rect2I() = default;

		constexpr Rect2I(INT32 x, INT32 y, UINT32 width, UINT32 height)
			:x(x), y(y), width(width), height(height)
		{ }

		INT32 x = 0;
		INT32 y = 0;
		UINT32 width = 0;
		UINT32 height = 0;

		/** Returns true if the rectangle contains the provided point. */
		bool contains(const Vector2I& point) const;

		/**
		 * Returns true if the rectangle overlaps the provided rectangle. Also returns true if the rectangles are contained
		 * within each other completely (no intersecting edges).
		 */
		bool overlaps(const Rect2I& other) const;

		/** Extends this rectangle so that the provided rectangle is completely contained within it. */
		void encapsulate(const Rect2I& other);

		/** Clips current rectangle so that it does not overlap the provided rectangle. */
		void clip(const Rect2I& clipRect);

		/**
		 * Cuts the current rectangle with the provided rectangle and outputs the pieces. The pieces will contain all area
		 * of the current rectangle without including the cut rectangle area.
		 */
		void cut(const Rect2I& cutRect, Vector<Rect2I>& pieces);

		/**
		 * Cuts the current rectangle with the provided rectangles and outputs the pieces. The pieces will contain all area
		 * of the current rectangle without including the cut rectangles area.
		 */
		void cut(const Vector<Rect2I>& cutRects, Vector<Rect2I>& pieces);

		/**
		 * Transforms the bounds by the given matrix. Resulting value is an axis aligned rectangle encompassing the
		 * transformed points.
		 * 			
		 * @note	
		 * Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds will be larger
		 * than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		bool operator== (const Rect2I& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		bool operator!= (const Rect2I& rhs) const
		{
			return !(*this == rhs);
		}

		static const Rect2I EMPTY;
	};

	/** @} */
}

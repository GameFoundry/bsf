#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a 2D rectangle using integer values.
	 * 			Rectangle is represented with an origin in top left and width/height.
	 */
	class BS_UTILITY_EXPORT Rect2I
	{
	public:
		Rect2I();
		Rect2I(int _x, int _y, int _width, int _height);

		int x, y, width, height;

		/**
		 * @brief	Returns true if the rectangle contains the provided point.
		 */
		bool contains(const Vector2I& point) const;

		/**
		 * @brief	Returns true if the rectangle overlaps the provided rectangle.
		 * 			Also returns true if the rectangles are contained within each other completely
		 * 			(no intersecting edges).
		 */
		bool overlaps(const Rect2I& other) const;

		/**
		 * @brief	Extends this rectangle so that the provided rectangle is
		 * 			completely contained within it.
		 */
		void encapsulate(const Rect2I& other);

		/**
		 * @brief	Clips current rectangle so that it does not overlap
		 * 			the provided rectangle.
		 */
		void clip(const Rect2I& clipRect);

		/**
		 * @brief	Cuts the current rectangle with the provided rectangle and outputs
		 *			the pieces. The pieces will contain all area of the current rectangle
		 *			without including the cut rectangle area.
		 */
		void cut(const Rect2I& cutRect, Vector<Rect2I>& pieces);

		/**
		 * @brief	Cuts the current rectangle with the provided rectangles and outputs
		 *			the pieces. The pieces will contain all area of the current rectangle
		 *			without including the cut rectangles area.
		 */
		void cut(const Vector<Rect2I>& cutRects, Vector<Rect2I>& pieces);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		inline bool operator== (const Rect2I& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		inline bool operator!= (const Rect2I& rhs) const
		{
			return !(*this == rhs);
		}

		static const Rect2I EMPTY;
	};
}
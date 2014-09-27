#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a 2D rectangle using real values.
	 * 			Rectangle is represented with an origin and size values.
	 */
	class BS_UTILITY_EXPORT Rect2
	{
	public:
		Rect2();
		Rect2(float _x, float _y, float _width, float _height);

		float x, y, width, height;

		/**
		 * @brief	Returns true if the rectangle contains the provided point.
		 */
		bool contains(const Vector2& point) const;

		/**
		 * @brief	Returns true if the rectangle overlaps the provided rectangle.
		 * 			Also returns true if the rectangles are contained within each other completely
		 * 			(no intersecting edges).
		 */
		bool overlaps(const Rect2& other) const;

		/**
		 * @brief	Extends this rectangle so that the provided rectangle is
		 * 			completely contained within it.
		 */
		void encapsulate(const Rect2& other);

		/**
		 * @brief	Clips current rectangle so that it does not overlap
		 * 			the provided rectangle.
		 */
		void clip(const Rect2& clipRect);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		inline bool operator== (const Rect2& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		inline bool operator!= (const Rect2& rhs) const
		{
			return !(*this == rhs);
		}

		static const Rect2 EMPTY;
	};
}
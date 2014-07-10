//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a 2D rectangle using real values.
	 * 			Rectangle is represented with an origin and size values.
	 */
	class BS_UTILITY_EXPORT RectF
	{
	public:
		RectF();
		RectF(float _x, float _y, float _width, float _height);

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
		bool overlaps(const RectF& other) const;

		/**
		 * @brief	Extends this rectangle so that the provided rectangle is
		 * 			completely contained within it.
		 */
		void encapsulate(const RectF& other);

		/**
		 * @brief	Clips current rectangle so that it does not overlap
		 * 			the provided rectangle.
		 */
		void clip(const RectF& clipRect);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		inline bool operator== (const RectF& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		inline bool operator!= (const RectF& rhs) const
		{
			return !(*this == rhs);
		}

		static const RectF EMPTY;
	};
}
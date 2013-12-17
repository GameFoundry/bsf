#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT RectI
	{
	public:
		RectI();
		RectI(int _x, int _y, int _width, int _height);

		int x, y, width, height;

		bool contains(const Vector2I& point) const;
		bool overlaps(const RectI& other) const;
		void encapsulate(const RectI& other);
		void clip(const RectI& clipRect);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		inline bool operator== (const RectI& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		inline bool operator!= (const RectI& rhs) const
		{
			return !(*this == rhs);
		}

		static const RectI EMPTY;
	};
}
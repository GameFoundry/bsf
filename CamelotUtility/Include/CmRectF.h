#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT RectF
	{
	public:
		RectF();
		RectF(float _x, float _y, float _width, float _height);

		float x, y, width, height;

		bool contains(const Vector2& point) const;
		bool overlaps(const RectF& other) const;
		void encapsulate(const RectF& other);
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
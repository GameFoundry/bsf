#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT FRect
	{
	public:
		FRect();
		FRect(float _x, float _y, float _width, float _height);

		float x, y, width, height;

		bool contains(const Vector2& point) const;
		bool overlaps(const FRect& other) const;
		void encapsulate(const FRect& other);
		void clip(const FRect& clipRect);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		inline bool operator== (const FRect& rhs) const
		{
			return x == rhs.x && y == rhs.y && width == rhs.width && height == rhs.height;
		}

		inline bool operator!= (const FRect& rhs) const
		{
			return !(*this == rhs);
		}

		static const FRect EMPTY;
	};
}
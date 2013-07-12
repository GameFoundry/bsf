#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	class CM_UTILITY_EXPORT Rect
	{
	public:
		Rect();
		Rect(int _x, int _y, int _width, int _height);

		bool contains(const Int2& point) const;
		bool overlaps(const Rect& other) const;
		void encapsulate(const Rect& other);
		void clip(const Rect& clipRect);

		/**
		 * @brief	Transforms the bounds by the given matrix.
		 * 			Resulting value is an axis aligned rectangle encompassing the transformed points.
		 * 			
		 * @note	Since the resulting value is an AA rectangle of the original transformed rectangle, the bounds
		 * 			will be larger than needed. Oriented rectangle would provide a much tighter fit.
		 */
		void transform(const Matrix4& matrix);

		int x, y, width, height;
	};
}
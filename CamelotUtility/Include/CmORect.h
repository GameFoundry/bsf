#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmVector2.h"
#include "CmRect.h"
#include "CmMatrix4.h"

namespace CamelotFramework
{
	/**
	 * @brief	Oriented rectangle. Can be rotated/translated/scaled. Skewing and similar transformations
	 * 			are not supported.
	 * 			TODO - Can be made faster. Speed wasn't my concern when designing the class.
	 */
	class CM_UTILITY_EXPORT ORect
	{
	public:
		ORect();
		ORect(const Rect& rect);
		
		void applyTransform(const Matrix4& tfrm);
		bool contains(const Vector2& point);

		Vector2 getMin() const;
		Vector2 getMax() const;

	private:
		Vector2 mOrigin;
		Vector2 mSides[2];
		float mSideLengths[2]; // Redundant but I want my side normals to be normalized
	};
}
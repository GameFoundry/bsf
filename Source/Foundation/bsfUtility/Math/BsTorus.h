//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Math/BsVector3.h"

namespace bs
{
	/** @addtogroup Math
	 *  @{
	 */

	/**
	 * Represents a torus at the world center. Outer radius represents the distance from the center, and inner radius
	 * represents the radius of the tube. Inner radius must be less or equal than the outer radius.
	 */
	class BS_UTILITY_EXPORT Torus
	{
	public:
		Torus() = default;

		Torus(const Vector3& normal, float outerRadius, float innerRadius)
			:normal(normal), outerRadius(outerRadius), innerRadius(innerRadius)
		{ }

		/** Ray/torus intersection, returns boolean result and distance to nearest intersection point. */
		std::pair<bool, float> intersects(const Ray& ray) const;

		Vector3 normal{BsZero};
		float outerRadius = 0.0f;
		float innerRadius = 0.0f;
	};

	/** @} */
}

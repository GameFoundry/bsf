#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a torus at the world center. Outer radius represents
	 *			the distance from the center, and inner radius represents the radius of the tube.
	 *			Inner radius must be less or equal than the outer radius.
	 */
	class BS_UTILITY_EXPORT Torus
	{
	public:
		Torus();
		Torus(float outerRadius, float innerRadius);

		/**
		 * @brief	Ray/torus intersection, returns boolean result and distance to nearest intersection point.
		 */
		std::pair<bool, float> intersects(const Ray& ray) const;

		float outerRadius;
		float innerRadius;
	};
}
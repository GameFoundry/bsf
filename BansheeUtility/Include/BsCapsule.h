#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsVector3.h"
#include "BsLineSegment3.h"

namespace BansheeEngine
{
	/** @addtogroup Math
	 *  @{
	 */

	/** Represents a capsule represented by a line segment and a radius. */
	class BS_UTILITY_EXPORT Capsule
	{
	public:
		Capsule();
		Capsule(const LineSegment3& segment, float radius);

		/**
		 * Ray/capsule intersection.
		 *
		 * @return	Boolean result and distance to the nearest intersection point.
		 */
		std::pair<bool, float> intersects(const Ray& ray) const;

		/**
		 * Returns the line segment along which the capsule lies. 
		 * All capsule points are at equal distance from this segment.
		 */
		const LineSegment3& getSegment() const { return mSegment; }

		/** Returns the radius of the capsule. It defines the distance of the capsule from its line segment. */
		float getRadius() const { return mRadius; }

	private:
		LineSegment3 mSegment;
		float mRadius;
	};

	/** @} */
}
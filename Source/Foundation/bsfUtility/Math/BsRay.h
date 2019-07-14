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

	/** A ray in 3D space represented with an origin and direction. */
	class BS_UTILITY_EXPORT Ray
	{
	public:
		Ray() = default;

		Ray(const Vector3& origin, const Vector3& direction)
			:mOrigin(origin), mDirection(direction)
		{ }

		void setOrigin(const Vector3& origin) { mOrigin = origin; }
		const Vector3& getOrigin() const { return mOrigin; }

		void setDirection(const Vector3& dir) { mDirection = dir; }
		const Vector3& getDirection() const {return mDirection;}

		/** Gets the position of a point t units along the ray. */
		Vector3 getPoint(float t) const
		{
			return Vector3(mOrigin + (mDirection * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector3 operator*(float t) const
		{
			return getPoint(t);
		}

		/** Transforms the ray by the given matrix. */
		void transform(const Matrix4& matrix);

		/**
		 * Transforms the ray by the given matrix.
		 *
		 * @note	Provided matrix must be affine.
		 */
		void transformAffine(const Matrix4& matrix);

		/** Ray/plane intersection, returns boolean result and distance to intersection point. */
		std::pair<bool, float> intersects(const Plane& p) const;

		/** Ray/sphere intersection, returns boolean result and distance to nearest intersection point. */
		std::pair<bool, float> intersects(const Sphere& s) const;

		/** Ray/axis aligned box intersection, returns boolean result and distance to nearest intersection point. */
		std::pair<bool, float> intersects(const AABox& box) const;

		/**
		 * Ray/triangle intersection, returns boolean result and distance to intersection point.
		 *
		 * @param[in]	a				Triangle first vertex.
		 * @param[in]	b				Triangle second vertex.
		 * @param[in]	c				Triangle third vertex.
		 * @param[in]	normal			The normal of the triangle. Doesn't need to be normalized.
		 * @param[in]	positiveSide	(optional) Should intersections with the positive side (normal facing) count.
		 * @param[in]	negativeSide	(optional) Should intersections with the negative side (opposite of normal facing) count.
		 * @return						Boolean result if intersection happened and distance to intersection point.
		 */
		std::pair<bool, float> intersects(const Vector3& a, const Vector3& b, const Vector3& c,
			const Vector3& normal, bool positiveSide = true, bool negativeSide = true) const;

	protected:
		Vector3 mOrigin{Vector3::ZERO};
		Vector3 mDirection{Vector3::UNIT_Z};
	};

	/** @} */
}

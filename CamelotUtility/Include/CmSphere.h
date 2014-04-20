#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmVector3.h"

namespace BansheeEngine 
{
    /**
     * @brief	A sphere represented by a center point and a radius.
     */
    class CM_UTILITY_EXPORT Sphere
    {
    public:
        /**
         * @brief	Default constructor. Creates a unit sphere around the origin.
         */
        Sphere() : mRadius(1.0), mCenter(Vector3::ZERO) 
		{ }

        Sphere(const Vector3& center, float radius)
            :mRadius(radius), mCenter(center) 
		{ }

        /**
         * @brief	Returns the radius of the sphere.
         */
        float getRadius(void) const { return mRadius; }

        /**
         * @brief	Sets the radius of the sphere.
         */
        void setRadius(float radius) { mRadius = radius; }

        /**
         * @brief	Returns the center point of the sphere.
         */
        const Vector3& getCenter(void) const { return mCenter; }

        /**
         * @brief	Sets the center point of the sphere.
         */
        void setCenter(const Vector3& center) { mCenter = center; }

		/**
		 * @brief	Returns whether or not this sphere contains the provided point.
		 */
		bool contains(const Vector3& v) const
		{
            return ((v - mCenter).squaredLength() <= Math::sqr(mRadius));
		}

		/**
		 * @brief	Returns whether or not this sphere intersects another sphere.
		 */
		bool intersects(const Sphere& s) const
		{
            return (s.mCenter - mCenter).squaredLength() <=
                Math::sqr(s.mRadius + mRadius);
		}

		/**
		 * @brief	Returns whether or not this sphere intersects a box.
		 */
		bool intersects(const AABox& box) const;

		/**
		 * @brief	Returns whether or not this sphere intersects a plane.
		 */
		bool intersects(const Plane& plane) const;

		/**
		 * @brief	Ray/sphere intersection, returns boolean result and distance to nearest intersection.
		 * 			
		 * @param	discardInside	(optional) If true the intersection will be discarded if ray origin
		 * 							is located within the sphere.
		 */
		std::pair<bool, float> intersects(const Ray& ray, bool discardInside = true) const;

	private:
		float mRadius;
		Vector3 mCenter;
    };
}
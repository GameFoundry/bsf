/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmVector3.h"

namespace CamelotFramework 
{
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
		 * @brief	Returns whether or not this sphere intersects a point.
		 */
		bool intersects(const Vector3& v) const
		{
            return ((v - mCenter).squaredLength() <= Math::sqr(mRadius));
		}

		/**
		 * @brief	Ray / sphere intersection, returns boolean result and distance.
		 */
		std::pair<bool, float> intersects(const Ray& ray, bool discardInside = true) const;

	private:
		float mRadius;
		Vector3 mCenter;
    };
}
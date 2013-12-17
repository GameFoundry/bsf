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
// This file is based on material originally from:
// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt

#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmVector3.h"

namespace CamelotFramework 
{
    class CM_UTILITY_EXPORT Plane
    {
	public:
		/**
		 * @brief	The "positive side" of the plane is the half space to which the
		 *			plane normal points. The "negative side" is the other half
		 *			space. The flag "no side" indicates the plane itself.
         */
        enum Side
        {
            NO_SIDE,
            POSITIVE_SIDE,
            NEGATIVE_SIDE,
            BOTH_SIDE
        };

    public:
        Plane();
        Plane(const Plane& copy);
        Plane(const Vector3& normal, float d);
		Plane(float a, float b, float c, float d);
        Plane(const Vector3& normal, const Vector3& point);
        Plane(const Vector3& point0, const Vector3& point1, const Vector3& point2);

        /**
         * @brief	Returns the side of the plane where the point is located on.
         * 			
		 * @note	NO_SIDE signifies the point is on the plane.
         */
        Side getSide(const Vector3& point) const;

        /**
		 * @brief	Returns the side where the alignedBox is. The flag BOTH_SIDE indicates an intersecting box.
		 *			One corner ON the plane is sufficient to consider the box and the plane intersecting.
         */
        Side getSide(const AABox& box) const;

        /**
         * @brief	Returns a distance from point to plane.
         *
		 * @note	The sign of the return value is positive if the point is on the 
		 * 			positive side of the plane, negative if the point is on the negative 
		 * 			side, and zero if the point is on the plane.
         */
        float getDistance(const Vector3& point) const;

		/**
		 * @brief	Project a vector onto the plane.
		 */
		Vector3 projectVector(const Vector3& v) const;

        /**
		 * @brief	Normalizes the plane's normal and the length scale of d
		 *			is as well.
         */
        float normalize();

		/**
		 * @brief	Box/plane intersection.
		 */
		bool intersects(const AABox& box) const;

		/**
		 * @brief	Sphere/plane intersection.
		 */
		bool intersects(const Sphere& sphere) const;

		/**
		 * @brief	Ray/plane intersection, returns boolean result and distance.
		 */
		std::pair<bool, float> intersects(const Ray& ray) const;

        bool operator==(const Plane& rhs) const
        {
            return (rhs.d == d && rhs.normal == normal);
        }
        bool operator!=(const Plane& rhs) const
        {
            return (rhs.d != d || rhs.normal != normal);
        }

	public:
		Vector3 normal;
		float d;
    };

    typedef Vector<Plane>::type PlaneList;
}
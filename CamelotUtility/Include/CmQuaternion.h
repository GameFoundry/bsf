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
#include "CmMath.h"

namespace CamelotFramework 
{
    class CM_UTILITY_EXPORT Quaternion
    {
    public:
        Quaternion(float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f)
			:w(w), z(z), y(y), x(x)
		{ }

        /**
         * @brief	Construct a quaternion from a rotation matrix.
         */
        explicit Quaternion(const Matrix3& rot)
        {
            fromRotationMatrix(rot);
        }

        /**
         * @brief	Construct a quaternion from an angle/axis.
         */
        Quaternion(const Radian& angle, const Vector3& axis)
        {
            fromAngleAxis(angle, axis);
        }

        /**
         * @brief	Construct a quaternion from 3 orthonormal local axes.
         */
        Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            fromAxes(xaxis, yaxis, zaxis);
        }

		/**
		 * @brief	Exchange the contents of this quaternion with another.
		 */
		void swap(Quaternion& other)
		{
			std::swap(w, other.w);
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		float operator [] (const size_t i) const
		{
			assert(i < 4);

			return *(&w+i);
		}

		float& operator [] (const size_t i)
		{
			assert(i < 4);

			return *(&w+i);
		}

		inline float* ptr()
		{
			return &w;
		}

		inline const float* ptr() const
		{
			return &w;
		}

		void fromRotationMatrix(const Matrix3& mat);
        void toRotationMatrix(Matrix3& mat) const;

        void fromAngleAxis(const Radian& angle, const Vector3& axis);
        void toAngleAxis(Radian& angle, Vector3& axis) const;

        void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        void toAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

        /**
         * @brief	Get the local x-axis.
         */
        Vector3 xAxis() const;

        /**
         * @brief	Get the local y-axis.
         */
        Vector3 yAxis() const;

		/**
         * @brief	Get the local z-axis.
         */
        Vector3 zAxis() const;

        Quaternion& operator= (const Quaternion& rhs)
		{
			w = rhs.w;
			x = rhs.x;
			y = rhs.y;
			z = rhs.z;

			return *this;
		}

        Quaternion operator+ (const Quaternion& rhs) const;
        Quaternion operator- (const Quaternion& rhs) const;
        Quaternion operator* (const Quaternion& rhs) const;
        Quaternion operator* (float rhs) const;
        Quaternion operator- () const;

        bool operator== (const Quaternion& rhs) const
		{
			return (rhs.x == x) && (rhs.y == y) && (rhs.z == z) && (rhs.w == w);
		}

        bool operator!= (const Quaternion& rhs) const
		{
			return !operator==(rhs);
		}

		friend Quaternion operator* (float lhs, const Quaternion& rhs);

        float dot(const Quaternion& other) const;  

        /**
         * @brief	Normalizes this quaternion, and returns the previous length.
         */
        float normalize(); 

        /**
         * @brief	Gets the inverse.
         *
         * @note	Quaternion must be non-zero.
         */
        Quaternion inverse() const; 

        /**
         * @brief	Rotates the provided vector.
         */
        Vector3 rotate(const Vector3& vec) const;

        /**
         * @brief	Spherical linear interpolation
         */
        static Quaternion slerp(float t, const Quaternion& p,
            const Quaternion& q, bool shortestPath = false);

        static const float EPSILON;

        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

		float w, x, y, z;

		inline bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}
    };
}
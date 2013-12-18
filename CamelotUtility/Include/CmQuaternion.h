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
#include "CmVector3.h"

namespace CamelotFramework 
{
    class CM_UTILITY_EXPORT Quaternion
    {
	private:
		struct EulerAngleOrderData
		{
			int a, b, c;
		};

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
        explicit Quaternion(const Vector3& axis, const Radian& angle)
        {
            fromAxisAngle(axis, angle);
        }

        /**
         * @brief	Construct a quaternion from 3 orthonormal local axes.
         */
        explicit Quaternion(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
        {
            fromAxes(xaxis, yaxis, zaxis);
        }

        /**
         * @brief	Construct a quaternion from euler angles, XYZ ordering.
         * 			
		 * @see		Quaternion::fromEulerAngles
         */
		explicit Quaternion(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle)
		{
			fromEulerAngles(xAngle, yAngle, zAngle);
		}

        /**
         * @brief	Construct a quaternion from euler angles, custom ordering.
         * 			
		 * @see		Quaternion::fromEulerAngles
         */
		explicit Quaternion(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle, EulerAngleOrder order)
		{
			fromEulerAngles(xAngle, yAngle, zAngle, order);
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

		float operator[] (const size_t i) const
		{
			assert(i < 4);

			return *(&w+i);
		}

		float& operator[] (const size_t i)
		{
			assert(i < 4);

			return *(&w+i);
		}

		void fromRotationMatrix(const Matrix3& mat);
        void fromAxisAngle(const Vector3& axis, const Radian& angle);
        void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        
        /**
         * @brief	Creates a quaternion from the provided Pitch/Yaw/Roll angles.
         *
		 * @param	xAngle	Rotation about x axis. (AKA Pitch)
		 * @param	yAngle	Rotation about y axis. (AKA Yaw)
		 * @param	zAngle	Rotation about z axis. (AKA Roll)
         *
         * @note	Since different values will be produced depending in which order are the rotations applied, this method assumes
		 * 			they are applied in XYZ order. If you need a specific order, use the overloaded "fromEulerAngles" method instead.
         */
        void fromEulerAngles(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle);

        /**
         * @brief	Creates a quaternion from the provided Pitch/Yaw/Roll angles.
         *
		 * @param	xAngle	Rotation about x axis. (AKA Pitch)
		 * @param	yAngle	Rotation about y axis. (AKA Yaw)
		 * @param	zAngle	Rotation about z axis. (AKA Roll)
		 * @param	order 	The order in which rotations will be extracted.
		 * 					Different values can be retrieved depending on the order.
         */
        void fromEulerAngles(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle, EulerAngleOrder order);

		void toRotationMatrix(Matrix3& mat) const;
		void toAxisAngle(Vector3& axis, Radian& angle) const;
		void toAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

		/**
         * @brief	Extracts Pitch/Yaw/Roll rotations from this quaternion.
         *
         * @param [in,out]	xAngle	Rotation about x axis. (AKA Pitch)
         * @param [in,out]	yAngle  Rotation about y axis. (AKA Yaw)
         * @param [in,out]	zAngle 	Rotation about z axis. (AKA Roll)
         *
         * @return	True if unique solution was found, false otherwise.
         * 			
		 * @note	Since different values will be returned depending in which order are the rotations applied, this method assumes
		 * 			they are applied in XYZ order. If you need a specific order, use the overloaded "toEulerAngles" method instead.
         */
        bool toEulerAngles(Radian& xAngle, Radian& yAngle, Radian& zAngle) const;

		/**
		 * @brief	Extracts Pitch/Yaw/Roll rotations from this quaternion.
		 *
		 * @param	xAngle	Rotation about x axis. (AKA Pitch)
		 * @param	yAngle	Rotation about y axis. (AKA Yaw)
		 * @param	zAngle	Rotation about z axis. (AKA Roll)
		 * @param	order 	The order in which rotations will be extracted. 
		 * 					Different values can be retrieved depending on the order.
		 *
		 * @return	True if unique solution was found, false otherwise.
		 */
		bool toEulerAngles(Radian& xAngle, Radian& yAngle, Radian& zAngle, EulerAngleOrder order) const;

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

		bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}

        /**
         * @brief	Spherical linear interpolation
         */
        static Quaternion slerp(float t, const Quaternion& p,
            const Quaternion& q, bool shortestPath = false);

        /**
		 * @brief	Gets the shortest arc quaternion to rotate this vector to the destination
		 *		vector.
         */
        static Quaternion getRotationFromTo(const Vector3& from, const Vector3& dest, const Vector3& fallbackAxis = Vector3::ZERO);

        static const float EPSILON;

        static const Quaternion ZERO;
        static const Quaternion IDENTITY;

		float x, y, z, w;

		private:
			static const EulerAngleOrderData EA_LOOKUP[6];
    };
}
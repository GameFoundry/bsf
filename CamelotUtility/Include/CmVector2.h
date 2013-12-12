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
#include "CmMath.h"

namespace CamelotFramework
{
    class CM_UTILITY_EXPORT Vector2
    {
    public:
        float x, y;

    public:
        Vector2()
        { }

        Vector2(float x, float y)
            : x(x), y(y)
        { }

		/**
		 * @brief	Exchange the contents of this vector with another.
		 */
		void swap(Vector2& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
		}

		float operator[] (size_t i) const
        {
            assert(i < 2);

            return *(&x+i);
        }

		float& operator[] (size_t i)
        {
            assert(i < 2);

            return *(&x+i);
        }

		float* ptr()
		{
			return &x;
		}

		const float* ptr() const
		{
			return &x;
		}

        Vector2& operator= (const Vector2& rhs)
        {
            x = rhs.x;
            y = rhs.y;

            return *this;
        }

		Vector2& operator= (float rhs)
		{
			x = rhs;
			y = rhs;

			return *this;
		}

        bool operator== (const Vector2& rhs) const
        {
            return (x == rhs.x && y == rhs.y);
        }

        bool operator!= (const Vector2& rhs) const
        {
            return (x != rhs.x || y != rhs.y);
        }

        Vector2 operator+ (const Vector2& rhs) const
        {
            return Vector2(x + rhs.x, y + rhs.y);
        }

        Vector2 operator- (const Vector2& rhs) const
        {
            return Vector2(x - rhs.x, y - rhs.y);
        }

        Vector2 operator* (const float rhs) const
        {
            return Vector2(x * rhs, y * rhs);
        }

        Vector2 operator* (const Vector2& rhs) const
        {
            return Vector2(x * rhs.x, y * rhs.y);
        }

        Vector2 operator/ (const float rhs) const
        {
            assert(rhs != 0.0);

            float fInv = 1.0f / rhs;

            return Vector2(x * fInv, y * fInv);
        }

        Vector2 operator/ (const Vector2& rhs) const
        {
            return Vector2(x / rhs.x, y / rhs.y);
        }

        const Vector2& operator+ () const
        {
            return *this;
        }

        Vector2 operator- () const
        {
            return Vector2(-x, -y);
        }

        friend Vector2 operator* (float lhs, const Vector2& rhs)
        {
            return Vector2(lhs * rhs.x, lhs * rhs.y);
        }

        friend Vector2 operator/ (float lhs, const Vector2& rhs)
        {
            return Vector2(lhs / rhs.x, lhs / rhs.y);
        }

        friend Vector2 operator+ (Vector2& lhs, float rhs)
        {
            return Vector2(lhs.x + rhs, lhs.y + rhs);
        }

        friend Vector2 operator+ (float lhs, const Vector2& rhs)
        {
            return Vector2(lhs + rhs.x, lhs + rhs.y);
        }

        friend Vector2 operator- (const Vector2& lhs, float rhs)
        {
            return Vector2(lhs.x - rhs, lhs.y - rhs);
        }

        friend Vector2 operator- (const float lhs, const Vector2& rhs)
        {
            return Vector2(lhs - rhs.x, lhs - rhs.y);
        }

        Vector2& operator+= (const Vector2& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vector2& operator+= (float rhs)
        {
            x += rhs;
            y += rhs;

            return *this;
        }

        Vector2& operator-= (const Vector2& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        Vector2& operator-= (float rhs)
        {
            x -= rhs;
            y -= rhs;

            return *this;
        }

        Vector2& operator*= (float rhs)
        {
            x *= rhs;
            y *= rhs;

            return *this;
        }

        Vector2& operator*= (const Vector2& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;

            return *this;
        }

        Vector2& operator/= (float rhs)
        {
            assert(rhs != 0.0f);

            float inv = 1.0f / rhs;

            x *= inv;
            y *= inv;

            return *this;
        }

        Vector2& operator/= (const Vector2& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;

            return *this;
        }

        /**
         * @brief	Returns the length (magnitude) of the vector.
         */
        float length() const
        {
            return Math::Sqrt(x * x + y * y);
        }

        /**
         * @brief	Returns the square of the length(magnitude) of the vector.
         */
        float squaredLength() const
        {
            return x * x + y * y;
        }

        /**
         * @brief	Returns the distance to another vector.
         */
        float distance(const Vector2& rhs) const
        {
            return (*this - rhs).length();
        }

        /**
         * @brief	Returns the square of the distance to another vector.
         */
        float sqrdDistance(const Vector2& rhs) const
        {
            return (*this - rhs).squaredLength();
        }

        /**
         * @brief	Calculates the dot (scalar) product of this vector with another.
         */
        float dot(const Vector2& vec) const
        {
            return x * vec.x + y * vec.y;
        }

        /**
         * @brief	Normalizes the vector.
         */
        float normalize()
        {
            float len = Math::Sqrt(x * x + y * y);

            // Will also work for zero-sized vectors, but will change nothing
            if (len > 1e-08)
            {
                float invLen = 1.0f / len;
                x *= invLen;
                y *= invLen;
            }

            return len;
        }

        /**
         * @brief	Generates a vector perpendicular to this vector.
         */
        Vector2 perpendicular() const
        {
            return Vector2 (-y, x);
        }

        /**
		 * @brief	Calculates the 2 dimensional cross-product of 2 vectors, which results
		 *		in a single floating point value which is 2 times the area of the triangle.
         */
        float cross(const Vector2& other) const
        {
            return x * other.y - y * other.x;
        }

        /**
		 * @brief	Sets this vector's components to the minimum of its own and the
		 *		ones of the passed in vector.
         */
        void floor(const Vector2& cmp)
        {
            if(cmp.x < x) x = cmp.x;
            if(cmp.y < y) y = cmp.y;
        }

        /**
		 * @brief	Sets this vector's components to the maximum of its own and the
		 *		ones of the passed in vector.
         */
        void ceil(const Vector2& cmp)
        {
            if(cmp.x > x) x = cmp.x;
            if(cmp.y > y) y = cmp.y;
        }

        /**
         * @brief	Returns true if this vector is zero length.
         */
        bool isZeroLength() const
        {
            float sqlen = (x * x) + (y * y);
            return (sqlen < (1e-06 * 1e-06));
        }

        /**
		 * @brief	Calculates a reflection vector to the plane with the given normal.
         */
        Vector2 reflect(const Vector2& normal) const
        {
            return Vector2(*this - (2 * this->dot(normal) * normal));
        }

		static Vector2 normalize(const Vector2& val)
		{
			float len = Math::Sqrt(val.x * val.x + val.y * val.y);

			// Will also work for zero-sized vectors, but will change nothing
			Vector2 normalizedVec;
			if (len > 1e-08)
			{
				float invLen = 1.0f / len;
				normalizedVec.x *= invLen;
				normalizedVec.y *= invLen;
			}

			return normalizedVec;
		}

		bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y);
		}

		static Vector2 min(const Vector2& a, const Vector2& b)
		{
			return Vector2(std::min(a.x, b.x), std::min(a.y, b.y));
		}

		static Vector2 max(const Vector2& a, const Vector2& b)
		{
			return Vector2(std::max(a.x, b.x), std::max(a.y, b.y));
		}

        static const Vector2 ZERO;
		static const Vector2 ONE;
		static const Vector2 UNIT_X;
		static const Vector2 UNIT_Y;
    };

	CM_ALLOW_MEMCPY_SERIALIZATION(Vector2);
}
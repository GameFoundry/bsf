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
    class CM_UTILITY_EXPORT Vector4
    {
    public:
        float x, y, z, w;

    public:
        Vector4()
        { }

        Vector4(float x, float y, float z, float w)
            :x(x), y(y), z(z), w(w)
        { }

		/**
		 * @brief	Exchange the contents of this vector with another.
		 */
		void swap(Vector4& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
			std::swap(w, other.w);
		}
	
		float operator[] (size_t i) const
        {
            assert (i < 4);

            return *(&x+i);
        }

		float& operator[] (size_t i)
        {
            assert(i < 4);

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

        Vector4& operator= (const Vector4& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;

            return *this;
        }

		Vector4& operator= (float rhs)
		{
			x = rhs;
			y = rhs;
			z = rhs;
			w = rhs;

			return *this;
		}

        bool operator== (const Vector4& rhs) const
        {
            return (x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w);
        }

        bool operator!= (const Vector4& rhs) const
        {
            return (x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w);
        }

        Vector4& operator= (const Vector3& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = 1.0f;

            return *this;
        }

        Vector4 operator+ (const Vector4& rhs) const
        {
            return Vector4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }

        Vector4 operator- (const Vector4& rhs) const
        {
            return Vector4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }

        Vector4 operator* (float rhs) const
        {
            return Vector4(x * rhs, y * rhs, z * rhs, w * rhs);
        }

        Vector4 operator* (const Vector4& rhs) const
        {
            return Vector4(rhs.x * x, rhs.y * y, rhs.z * z, rhs.w * w);
        }

        Vector4 operator/ (float rhs) const
        {
            assert(rhs != 0.0f);

            float inv = 1.0f / rhs;
            return Vector4(x * inv, y * inv, z * inv, w * inv);
        }

        Vector4 operator/ (const Vector4& rhs) const
        {
            return Vector4(x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w);
        }

        const Vector4& operator+ () const
        {
            return *this;
        }

        Vector4 operator- () const
        {
            return Vector4(-x, -y, -z, -w);
        }

        friend Vector4 operator* (float lhs, const Vector4& rhs)
        {
            return Vector4(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z, lhs * rhs.w);
        }

        friend Vector4 operator/ (float lhs, const Vector4& rhs)
        {
            return Vector4(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
        }

        friend Vector4 operator+ (const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs);
        }

        friend Vector4 operator+ (float lhs, const Vector4& rhs)
        {
            return Vector4(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z, lhs + rhs.w);
        }

        friend Vector4 operator- (const Vector4& lhs, float rhs)
        {
            return Vector4(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs);
        }

        friend Vector4 operator- (float lhs, Vector4& rhs)
        {
            return Vector4(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z, lhs - rhs.w);
        }

        Vector4& operator+= (const Vector4& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;

            return *this;
        }

        Vector4& operator-= (const Vector4& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;

            return *this;
        }

        Vector4& operator*= (float rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;

            return *this;
        }

        Vector4& operator+= (float rhs)
        {
            x += rhs;
            y += rhs;
            z += rhs;
            w += rhs;

            return *this;
        }

        Vector4& operator-= (float rhs)
        {
            x -= rhs;
            y -= rhs;
            z -= rhs;
            w -= rhs;

            return *this;
        }

        Vector4& operator*= (Vector4& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;
            w *= rhs.w;

            return *this;
        }

        Vector4& operator/= (float rhs)
        {
            assert(rhs != 0.0f);

            float inv = 1.0f / rhs;

            x *= inv;
            y *= inv;
            z *= inv;
            w *= inv;

            return *this;
        }

        Vector4& operator/= (const Vector4& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;
            w /= rhs.w;

            return *this;
        }

        /**
         * @brief	Calculates the dot (scalar) product of this vector with another.
         */
        float dot(const Vector4& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z + w * vec.w;
        }

		bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}

        static const Vector4 ZERO;
    };

	CM_ALLOW_MEMCPY_SERIALIZATION(Vector4);
}


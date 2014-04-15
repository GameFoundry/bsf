#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	/**
	 * @brief	A two dimensional vector with integer
	 *			coordinates.
	 */
	struct CM_UTILITY_EXPORT Vector2I
	{
		INT32 x;
		INT32 y;

		Vector2I()
			:x(0), y(0)
		{ }

		inline Vector2I(INT32 _x, INT32 _y )
            :x(_x), y(_y)
        { }

        explicit Vector2I(int val)
            :x(val), y(val)
        { }

		/**
		 * @brief	Exchange the contents of this vector with another.
		 */
		void swap(Vector2I& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
		}

		/**
		 * @brief	Returns the manhattan distance between this and another vector.
		 */
		UINT32 manhattanDist(const Vector2I& other)
		{
			return (UINT32)fabs(float(other.x - x)) + (UINT32)fabs(float(other.y - y));
		}

		INT32 operator[] (size_t i) const
        {
            assert(i < 2);

            return *(&x+i);
        }

		INT32& operator[] (size_t i)
        {
            assert(i < 2);

            return *(&x+i);
        }

        Vector2I& operator= (const Vector2I& rhs)
        {
            x = rhs.x;
            y = rhs.y;

            return *this;
        }

		Vector2I& operator= (int val)
		{
			x = val;
			y = val;

			return *this;
		}

        bool operator== (const Vector2I& rhs) const
        {
            return (x == rhs.x && y == rhs.y);
        }

        bool operator!= (const Vector2I& rhs) const
        {
            return (x != rhs.x || y != rhs.y);
        }

        Vector2I operator+ (const Vector2I& rhs) const
        {
            return Vector2I(x + rhs.x, y + rhs.y);
        }

        Vector2I operator- (const Vector2I& rhs) const
        {
            return Vector2I(x - rhs.x, y - rhs.y);
        }

        Vector2I operator* (int val) const
        {
            return Vector2I(x * val, y * val);
        }

        Vector2I operator* (const Vector2I& rhs) const
        {
            return Vector2I(x * rhs.x, y * rhs.y);
        }

        Vector2I operator/ (int val) const
        {
            assert(val != 0);

            return Vector2I(x / val, y / val);
        }

        Vector2I operator/ (const Vector2I& rhs) const
        {
            return Vector2I(x / rhs.x, y / rhs.y);
        }

        const Vector2I& operator+ () const
        {
            return *this;
        }

        Vector2I operator- () const
        {
            return Vector2I(-x, -y);
        }

        friend Vector2I operator* (int lhs, const Vector2I& rhs)
        {
            return Vector2I(lhs * rhs.x, lhs * rhs.y);
        }

        friend Vector2I operator/ (int lhs, const Vector2I& rhs)
        {
            return Vector2I(lhs / rhs.x, lhs / rhs.y);
        }

        Vector2I& operator+= (const Vector2I& rhs)
        {
            x += rhs.x;
            y += rhs.y;

            return *this;
        }

        Vector2I& operator-= (const Vector2I& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;

            return *this;
        }

        Vector2I& operator*= (INT32 val)
        {
            x *= val;
            y *= val;

            return *this;
        }

        Vector2I& operator*= (const Vector2I& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;

            return *this;
        }

        Vector2I& operator/= (INT32 val)
        {
            assert(val != 0);

            x /= val;
            y /= val;

            return *this;
        }

        Vector2I& operator/= (const Vector2I& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;

            return *this;
        }
	};
}
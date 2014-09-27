#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsMath.h"

namespace BansheeEngine
{
	/**
	 * @brief	A three dimensional vector.
	 */
    class BS_UTILITY_EXPORT Vector3
    {
    public:
		float x, y, z;

    public:
        Vector3()
			:x(0.0f), y(0.0f), z(0.0f)
        { }

        Vector3(float x, float y, float z)
            :x(x), y(y), z(z)
        { }

		/**
		 * @brief	Exchange the contents of this vector with another.
		 */
		void swap(Vector3& other)
		{
			std::swap(x, other.x);
			std::swap(y, other.y);
			std::swap(z, other.z);
		}

		float operator[] (UINT32 i) const
        {
            assert(i < 3);

            return *(&x+i);
        }

		float& operator[] (UINT32 i)
        {
            assert(i < 3);

            return *(&x+i);
        }

		/** 
		 * @brief	Pointer accessor for direct copying.
		 */
		float* ptr()
		{
			return &x;
		}

		/** 
		 * @brief	Pointer accessor for direct copying.
		 */
		const float* ptr() const
		{
			return &x;
		}

        Vector3& operator= (const Vector3& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;

            return *this;
        }

        Vector3& operator= (float rhs)
        {
            x = rhs;
            y = rhs;
            z = rhs;

            return *this;
        }

        bool operator== (const Vector3& rhs) const
        {
            return (x == rhs.x && y == rhs.y && z == rhs.z);
        }

        bool operator!= (const Vector3& rhs) const
        {
            return (x != rhs.x || y != rhs.y || z != rhs.z);
        }

        Vector3 operator+ (const Vector3& rhs) const
        {
            return Vector3(x + rhs.x, y + rhs.y, z + rhs.z);
        }

        Vector3 operator- (const Vector3& rhs) const
        {
            return Vector3(x - rhs.x, y - rhs.y, z - rhs.z);
        }

        Vector3 operator* (float rhs) const
        {
            return Vector3(x * rhs, y * rhs, z * rhs);
        }

        Vector3 operator* (const Vector3& rhs) const
        {
            return Vector3(x * rhs.x, y * rhs.y, z * rhs.z);
        }

        Vector3 operator/ (float val) const
        {
            assert(val != 0.0);

            float fInv = 1.0f / val;
            return Vector3(x * fInv, y * fInv, z * fInv);
        }

        Vector3 operator/ (const Vector3& rhs) const
        {
            return Vector3(x / rhs.x, y / rhs.y, z / rhs.z);
        }

        const Vector3& operator+ () const
        {
            return *this;
        }

        Vector3 operator- () const
        {
            return Vector3(-x, -y, -z);
        }

        friend Vector3 operator* (float lhs, const Vector3& rhs)
        {
            return Vector3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
        }

        friend Vector3 operator/ (float lhs, const Vector3& rhs)
        {
            return Vector3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
        }

        friend Vector3 operator+ (const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x + rhs, lhs.y + rhs, lhs.z + rhs);
        }

        friend Vector3 operator+ (float lhs, const Vector3& rhs)
        {
            return Vector3(lhs + rhs.x, lhs + rhs.y, lhs + rhs.z);
        }

        friend Vector3 operator- (const Vector3& lhs, float rhs)
        {
            return Vector3(lhs.x - rhs, lhs.y - rhs, lhs.z - rhs);
        }

        friend Vector3 operator- (float lhs, const Vector3& rhs)
        {
            return Vector3(lhs - rhs.x, lhs - rhs.y, lhs - rhs.z);
        }

        Vector3& operator+= (const Vector3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;

            return *this;
        }

        Vector3& operator+= (float rhs)
        {
            x += rhs;
            y += rhs;
            z += rhs;

            return *this;
        }

        Vector3& operator-= (const Vector3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;

            return *this;
        }

        Vector3& operator-= (float rhs)
        {
            x -= rhs;
            y -= rhs;
            z -= rhs;

            return *this;
        }

        Vector3& operator*= (float rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;

            return *this;
        }

        Vector3& operator*= (const Vector3& rhs)
        {
            x *= rhs.x;
            y *= rhs.y;
            z *= rhs.z;

            return *this;
        }

        Vector3& operator/= (float rhs)
        {
            assert(rhs != 0.0f);

            float inv = 1.0f / rhs;

            x *= inv;
            y *= inv;
            z *= inv;

            return *this;
        }

        Vector3& operator/= (const Vector3& rhs)
        {
            x /= rhs.x;
            y /= rhs.y;
            z /= rhs.z;

            return *this;
        }

        /**
         * @brief	Returns the length (magnitude) of the vector.
         */
        float length() const
        {
            return Math::sqrt(x * x + y * y + z * z);
        }

        /**
         * @brief	Returns the square of the length(magnitude) of the vector.
         */
        float squaredLength() const
        {
            return x * x + y * y + z * z;
        }

        /**
         * @brief	Returns the distance to another vector.
         */
        float distance(const Vector3& rhs) const
        {
            return (*this - rhs).length();
        }

        /**
         * @brief	Returns the square of the distance to another vector.
         */
        float squaredDistance(const Vector3& rhs) const
        {
            return (*this - rhs).squaredLength();
        }

        /**
         * @brief	Calculates the dot (scalar) product of this vector with another
         */
        float dot(const Vector3& vec) const
        {
            return x * vec.x + y * vec.y + z * vec.z;
        }

        /**
         * @brief	Normalizes the vector.
         */
        float normalize()
        {
            float len = Math::sqrt(x * x + y * y + z * z);

            // Will also work for zero-sized vectors, but will change nothing
            if (len > 1e-08)
            {
                float invLen = 1.0f / len;
                x *= invLen;
                y *= invLen;
                z *= invLen;
            }

            return len;
        }

        /**
		 * @brief	Calculates the cross-product of 2 vectors, i.e. the vector that
		 *		lies perpendicular to them both.
         */
        Vector3 cross(const Vector3& other) const
        {
            return Vector3(
                y * other.z - z * other.y,
                z * other.x - x * other.z,
                x * other.y - y * other.x);
        }

        /**
		 * @brief	Sets this vector's components to the minimum of its own and the
		 *		ones of the passed in vector.
         */
        void floor(const Vector3& cmp)
        {
            if(cmp.x < x) x = cmp.x;
            if(cmp.y < y) y = cmp.y;
            if(cmp.z < z) z = cmp.z;
        }

        /**
		 * @brief	Sets this vector's components to the maximum of its own and the
		 *		ones of the passed in vector.
         */
        void ceil(const Vector3& cmp)
        {
            if(cmp.x > x) x = cmp.x;
            if(cmp.y > y) y = cmp.y;
            if(cmp.z > z) z = cmp.z;
        }

        /**   
        *  @brief	Generates a vector perpendicular to this vector.
        */
        Vector3 perpendicular() const
        {
            static const float squareZero = (float)(1e-06 * 1e-06);

            Vector3 perp = this->cross(Vector3::UNIT_X);

            if(perp.squaredLength() < squareZero)
                perp = this->cross(Vector3::UNIT_Y);

			perp.normalize();
            return perp;
        }

		/**
		 * @brief	Gets the angle between 2 vectors.
		 */
		Radian angleBetween(const Vector3& dest)
		{
			float lenProduct = length() * dest.length();

			// Divide by zero check
			if(lenProduct < 1e-6f)
				lenProduct = 1e-6f;

			float f = dot(dest) / lenProduct;

			f = Math::clamp(f, -1.0f, 1.0f);
			return Math::acos(f);

		}

        /**
         * @brief	Returns true if this vector is zero length.
         */
        bool isZeroLength() const
        {
            float sqlen = (x * x) + (y * y) + (z * z);
            return (sqlen < (1e-06 * 1e-06));
        }

        /**
		 * @brief	Calculates a reflection vector to the plane with the given normal.
         */
        Vector3 reflect(const Vector3& normal) const
        {
            return Vector3(*this - (2 * this->dot(normal) * normal));
        }

		/**
		 * @brief	Calculates two vectors orthonormal to the current vector, and
		 *			normalizes the current vector if not already.
		 */
		void orthogonalComplement(Vector3& a, Vector3& b)
		{
			if (fabs(a.x) > fabs(a.y))
				a = Vector3(-a.z, 0, a.x);
			else
				a = Vector3(0, a.z, -a.y);

			b = cross(a);

			orthonormalize(*this, a, b);
		}

		/**
		 * @brief	Performs Gram-Schmidt orthonormalization
		 */
		static void orthonormalize(Vector3& vec0, Vector3& vec1, Vector3& vec2)
		{
			vec0.normalize();

			float dot0 = vec0.dot(vec1); 
			vec1 -= dot0*vec0;
			vec1.normalize();

			float dot1 = vec1.dot(vec2);
			dot0 = vec0.dot(vec2);
			vec2 -= dot0*vec0 + dot1*vec1;
			vec2.normalize();
		}

		/**
		 * @brief	Normalizes the provided vector and returns a new normalized instance.
		 */
		static Vector3 normalize(const Vector3& val)
		{
			float len = Math::sqrt(val.x * val.x + val.y * val.y + val.z * val.z);

			// Will also work for zero-sized vectors, but will change nothing
			if (len > 1e-08)
			{
				float invLen = 1.0f / len;

				Vector3 normalizedVec;
				normalizedVec.x = val.x * invLen;
				normalizedVec.y = val.y * invLen;
				normalizedVec.z = val.z * invLen;

				return normalizedVec;
			}
			else
				return val;
		}

		/**
		 * @brief	Checks are any of the vector components NaN.
		 */
		bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z);
		}

		/**
		 * @brief	Returns the minimum of all the vector components as a 
		 *			new vector.
		 */
		static Vector3 min(const Vector3& a, const Vector3& b)
		{
			return Vector3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z));
		}

		/**
		 * @brief	Returns the maximum of all the vector components as a 
		 *			new vector.
		 */
		static Vector3 max(const Vector3& a, const Vector3& b)
		{
			return Vector3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z));
		}

        static const Vector3 ZERO;
		static const Vector3 ONE;
        static const Vector3 UNIT_X;
        static const Vector3 UNIT_Y;
        static const Vector3 UNIT_Z;
    };

	BS_ALLOW_MEMCPY_SERIALIZATION(Vector3);
}

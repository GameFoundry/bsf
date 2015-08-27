#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsMath.h"
#include "BsVector3.h"

namespace BansheeEngine 
{
    /**
     * @brief	Represents a quaternion used for 3D rotations.
     */
    class BS_UTILITY_EXPORT Quaternion
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
         * @brief	Construct a quaternion from euler angles, YXZ ordering.
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

		/**
		 * @brief	Initializes the quaternion from a 3x3 rotation matrix.
		 * 			
		 * @note	It's up to the caller to ensure the matrix is orthonormal.
		 */
		void fromRotationMatrix(const Matrix3& mat);

		/**
		 * @brief	Initializes the quaternion from an angle axis pair. Quaternion
		 * 			will represent a rotation of "angle" radians around "axis".
		 */
        void fromAxisAngle(const Vector3& axis, const Radian& angle);

        /**
         * @brief	Initializes the quaternion from orthonormal set of axes. Quaternion
         * 			will represent a rotation from base axes to the specified set of axes.
         * 			
		 * @note	It's up to the caller to ensure the axes are orthonormal.
         */
        void fromAxes(const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis);
        
        /**
         * @brief	Creates a quaternion from the provided Pitch/Yaw/Roll angles.
         *
		 * @param	xAngle	Rotation about x axis. (AKA Pitch)
		 * @param	yAngle	Rotation about y axis. (AKA Yaw)
		 * @param	zAngle	Rotation about z axis. (AKA Roll)
         *
         * @note	Since different values will be produced depending in which order are the rotations applied, this method assumes
		 * 			they are applied in YXZ order. If you need a specific order, use the overloaded "fromEulerAngles" method instead.
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

		/**
		 * @brief	Converts a quaternion to a rotation matrix.
		 */
		void toRotationMatrix(Matrix3& mat) const;

		/**
		 * @brief	Converts a quaternion to an angle axis pair.
		 *
		 * @param [out]	axis 	The axis around the which rotation takes place.
		 * @param [out]	angle	The angle in radians determining amount of rotation around the axis.
		 */
		void toAxisAngle(Vector3& axis, Radian& angle) const;

		/**
		 * @brief	Converts a quaternion to an orthonormal set of axes.
		 *
		 * @param [out]	xAxis	The X axis.
		 * @param [out]	yAxis	The Y axis.
		 * @param [out]	zAxis	The Z axis.
		 */
		void toAxes(Vector3& xAxis, Vector3& yAxis, Vector3& zAxis) const;

		/**
         * @brief	Extracts Pitch/Yaw/Roll rotations from this quaternion.
         *
         * @param [out]	xAngle	Rotation about x axis. (AKA Pitch)
         * @param [out]	yAngle  Rotation about y axis. (AKA Yaw)
         * @param [out]	zAngle 	Rotation about z axis. (AKA Roll)
         *
         * @return	True if unique solution was found, false otherwise.
         */
        bool toEulerAngles(Radian& xAngle, Radian& yAngle, Radian& zAngle) const;

        /**
         * @brief	Gets the positive x-axis of the coordinate system transformed by this quaternion.
         */
        Vector3 xAxis() const;

        /**
         * @brief	Gets the positive y-axis of the coordinate system transformed by this quaternion.
         */
        Vector3 yAxis() const;

		/**
         * @brief	Gets the positive z-axis of the coordinate system transformed by this quaternion.
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

        /**
         * @brief	Calculates the dot product of this quaternion and another.
         */
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
		* @brief	Query if any of the components of the quaternion are NaN.
		 */
		bool isNaN() const
		{
			return Math::isNaN(x) || Math::isNaN(y) || Math::isNaN(z) || Math::isNaN(w);
		}

        /**
         * @brief	Performs spherical interpolation between two quaternions. Spherical interpolation 
         *          neatly interpolates between two rotations without modifying the size of the vector 
         *          it is applied to (unlike linear interpolation).
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

	BS_ALLOW_MEMCPY_SERIALIZATION(Quaternion);
}
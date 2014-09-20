#pragma once

#include "BsPrerequisitesUtil.h"

#include "BsVector3.h"
#include "BsMatrix3.h"
#include "BsVector4.h"
#include "BsPlane.h"

namespace BansheeEngine
{
	 /**
     * @brief	Class representing a 4x4 matrix.
     */
    class BS_UTILITY_EXPORT Matrix4
    {
    private:
        union 
		{
            float m[4][4];
            float _m[16];
        };

    public:
        Matrix4()
        { }

        Matrix4(
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33)
        {
            m[0][0] = m00;
            m[0][1] = m01;
            m[0][2] = m02;
            m[0][3] = m03;
            m[1][0] = m10;
            m[1][1] = m11;
            m[1][2] = m12;
            m[1][3] = m13;
            m[2][0] = m20;
            m[2][1] = m21;
            m[2][2] = m22;
            m[2][3] = m23;
            m[3][0] = m30;
            m[3][1] = m31;
            m[3][2] = m32;
            m[3][3] = m33;
        }

		Matrix4(const Matrix4& mat)
		{
			memcpy(_m, mat._m, 16*sizeof(float));
		}

        /**
         * @brief	Creates a 4x4 transformation matrix with a zero translation part from a rotation/scaling 3x3 matrix.
         */
        explicit Matrix4(const Matrix3& mat3)
        {
			m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2]; m[0][3] = 0.0f;
			m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2]; m[1][3] = 0.0f;
			m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2]; m[2][3] = 0.0f;
			m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
        }

        /**
         * @brief	Creates a 4x4 transformation matrix with translation, rotation and scale.
         */
        Matrix4(const Vector3& translation, const Quaternion& rot, const Vector3& scale)
        {
			setTRS(translation, rot, scale);
        }
        
		/**
		 * @brief	Swaps the contents of this matrix with another.
		 */
		void swap(Matrix4& other)
		{
			std::swap(m[0][0], other.m[0][0]);
			std::swap(m[0][1], other.m[0][1]);
			std::swap(m[0][2], other.m[0][2]);
			std::swap(m[0][3], other.m[0][3]);
			std::swap(m[1][0], other.m[1][0]);
			std::swap(m[1][1], other.m[1][1]);
			std::swap(m[1][2], other.m[1][2]);
			std::swap(m[1][3], other.m[1][3]);
			std::swap(m[2][0], other.m[2][0]);
			std::swap(m[2][1], other.m[2][1]);
			std::swap(m[2][2], other.m[2][2]);
			std::swap(m[2][3], other.m[2][3]);
			std::swap(m[3][0], other.m[3][0]);
			std::swap(m[3][1], other.m[3][1]);
			std::swap(m[3][2], other.m[3][2]);
			std::swap(m[3][3], other.m[3][3]);
		}

		/**
         * @brief	Returns a row of the matrix.
         */
		float* operator[] (UINT32 row)
        {
            assert(row < 4);

            return m[row];
        }

        const float *operator[] (UINT32 row) const
        {
            assert(row < 4);

            return m[row];
        }

        Matrix4 operator* (const Matrix4 &rhs) const
        {
			Matrix4 r;

			r.m[0][0] = m[0][0] * rhs.m[0][0] + m[0][1] * rhs.m[1][0] + m[0][2] * rhs.m[2][0] + m[0][3] * rhs.m[3][0];
			r.m[0][1] = m[0][0] * rhs.m[0][1] + m[0][1] * rhs.m[1][1] + m[0][2] * rhs.m[2][1] + m[0][3] * rhs.m[3][1];
			r.m[0][2] = m[0][0] * rhs.m[0][2] + m[0][1] * rhs.m[1][2] + m[0][2] * rhs.m[2][2] + m[0][3] * rhs.m[3][2];
			r.m[0][3] = m[0][0] * rhs.m[0][3] + m[0][1] * rhs.m[1][3] + m[0][2] * rhs.m[2][3] + m[0][3] * rhs.m[3][3];

			r.m[1][0] = m[1][0] * rhs.m[0][0] + m[1][1] * rhs.m[1][0] + m[1][2] * rhs.m[2][0] + m[1][3] * rhs.m[3][0];
			r.m[1][1] = m[1][0] * rhs.m[0][1] + m[1][1] * rhs.m[1][1] + m[1][2] * rhs.m[2][1] + m[1][3] * rhs.m[3][1];
			r.m[1][2] = m[1][0] * rhs.m[0][2] + m[1][1] * rhs.m[1][2] + m[1][2] * rhs.m[2][2] + m[1][3] * rhs.m[3][2];
			r.m[1][3] = m[1][0] * rhs.m[0][3] + m[1][1] * rhs.m[1][3] + m[1][2] * rhs.m[2][3] + m[1][3] * rhs.m[3][3];

			r.m[2][0] = m[2][0] * rhs.m[0][0] + m[2][1] * rhs.m[1][0] + m[2][2] * rhs.m[2][0] + m[2][3] * rhs.m[3][0];
			r.m[2][1] = m[2][0] * rhs.m[0][1] + m[2][1] * rhs.m[1][1] + m[2][2] * rhs.m[2][1] + m[2][3] * rhs.m[3][1];
			r.m[2][2] = m[2][0] * rhs.m[0][2] + m[2][1] * rhs.m[1][2] + m[2][2] * rhs.m[2][2] + m[2][3] * rhs.m[3][2];
			r.m[2][3] = m[2][0] * rhs.m[0][3] + m[2][1] * rhs.m[1][3] + m[2][2] * rhs.m[2][3] + m[2][3] * rhs.m[3][3];

			r.m[3][0] = m[3][0] * rhs.m[0][0] + m[3][1] * rhs.m[1][0] + m[3][2] * rhs.m[2][0] + m[3][3] * rhs.m[3][0];
			r.m[3][1] = m[3][0] * rhs.m[0][1] + m[3][1] * rhs.m[1][1] + m[3][2] * rhs.m[2][1] + m[3][3] * rhs.m[3][1];
			r.m[3][2] = m[3][0] * rhs.m[0][2] + m[3][1] * rhs.m[1][2] + m[3][2] * rhs.m[2][2] + m[3][3] * rhs.m[3][2];
			r.m[3][3] = m[3][0] * rhs.m[0][3] + m[3][1] * rhs.m[1][3] + m[3][2] * rhs.m[2][3] + m[3][3] * rhs.m[3][3];

			return r;
        }

        Matrix4 operator+ (const Matrix4 &rhs) const
        {
            Matrix4 r;

            r.m[0][0] = m[0][0] + rhs.m[0][0];
            r.m[0][1] = m[0][1] + rhs.m[0][1];
            r.m[0][2] = m[0][2] + rhs.m[0][2];
            r.m[0][3] = m[0][3] + rhs.m[0][3];

            r.m[1][0] = m[1][0] + rhs.m[1][0];
            r.m[1][1] = m[1][1] + rhs.m[1][1];
            r.m[1][2] = m[1][2] + rhs.m[1][2];
            r.m[1][3] = m[1][3] + rhs.m[1][3];

            r.m[2][0] = m[2][0] + rhs.m[2][0];
            r.m[2][1] = m[2][1] + rhs.m[2][1];
            r.m[2][2] = m[2][2] + rhs.m[2][2];
            r.m[2][3] = m[2][3] + rhs.m[2][3];

            r.m[3][0] = m[3][0] + rhs.m[3][0];
            r.m[3][1] = m[3][1] + rhs.m[3][1];
            r.m[3][2] = m[3][2] + rhs.m[3][2];
            r.m[3][3] = m[3][3] + rhs.m[3][3];

            return r;
        }

        Matrix4 operator- (const Matrix4 &rhs) const
        {
            Matrix4 r;
            r.m[0][0] = m[0][0] - rhs.m[0][0];
            r.m[0][1] = m[0][1] - rhs.m[0][1];
            r.m[0][2] = m[0][2] - rhs.m[0][2];
            r.m[0][3] = m[0][3] - rhs.m[0][3];

            r.m[1][0] = m[1][0] - rhs.m[1][0];
            r.m[1][1] = m[1][1] - rhs.m[1][1];
            r.m[1][2] = m[1][2] - rhs.m[1][2];
            r.m[1][3] = m[1][3] - rhs.m[1][3];

            r.m[2][0] = m[2][0] - rhs.m[2][0];
            r.m[2][1] = m[2][1] - rhs.m[2][1];
            r.m[2][2] = m[2][2] - rhs.m[2][2];
            r.m[2][3] = m[2][3] - rhs.m[2][3];

            r.m[3][0] = m[3][0] - rhs.m[3][0];
            r.m[3][1] = m[3][1] - rhs.m[3][1];
            r.m[3][2] = m[3][2] - rhs.m[3][2];
            r.m[3][3] = m[3][3] - rhs.m[3][3];

            return r;
        }

        inline bool operator== (const Matrix4& rhs ) const
        {
            if(m[0][0] != rhs.m[0][0] || m[0][1] != rhs.m[0][1] || m[0][2] != rhs.m[0][2] || m[0][3] != rhs.m[0][3] ||
               m[1][0] != rhs.m[1][0] || m[1][1] != rhs.m[1][1] || m[1][2] != rhs.m[1][2] || m[1][3] != rhs.m[1][3] ||
               m[2][0] != rhs.m[2][0] || m[2][1] != rhs.m[2][1] || m[2][2] != rhs.m[2][2] || m[2][3] != rhs.m[2][3] ||
               m[3][0] != rhs.m[3][0] || m[3][1] != rhs.m[3][1] || m[3][2] != rhs.m[3][2] || m[3][3] != rhs.m[3][3] )
			{
                return false;
			}

            return true;
        }

        inline bool operator!= (const Matrix4& rhs) const
        {
            return !operator==(rhs);
        }

		Matrix4 operator*(float rhs) const
		{
			return Matrix4(rhs*m[0][0], rhs*m[0][1], rhs*m[0][2], rhs*m[0][3],
						   rhs*m[1][0], rhs*m[1][1], rhs*m[1][2], rhs*m[1][3],
						   rhs*m[2][0], rhs*m[2][1], rhs*m[2][2], rhs*m[2][3],
						   rhs*m[3][0], rhs*m[3][1], rhs*m[3][2], rhs*m[3][3]);
		}

        /**
         * @brief	Returns a transpose of the matrix (switched columns and rows).
         */
        Matrix4 transpose() const
        {
            return Matrix4(m[0][0], m[1][0], m[2][0], m[3][0],
                           m[0][1], m[1][1], m[2][1], m[3][1],
                           m[0][2], m[1][2], m[2][2], m[3][2],
                           m[0][3], m[1][3], m[2][3], m[3][3]);
        }

        /**
         * @brief	Extracts the rotation/scaling part of the matrix as a 3x3 matrix.
         */
        void extract3x3Matrix(Matrix3& m3x3) const
        {
            m3x3.m[0][0] = m[0][0];
            m3x3.m[0][1] = m[0][1];
            m3x3.m[0][2] = m[0][2];
            m3x3.m[1][0] = m[1][0];
            m3x3.m[1][1] = m[1][1];
            m3x3.m[1][2] = m[1][2];
            m3x3.m[2][0] = m[2][0];
            m3x3.m[2][1] = m[2][1];
            m3x3.m[2][2] = m[2][2];
        }

		/**
		 * @brief	Calculates the adjoint of the matrix.
		 */
		Matrix4 adjoint() const;

		/**
		 * @brief	Calculates the determinant of the matrix.
		 */
		float determinant() const;

		/**
		 * @brief	Calculates the inverse of the matrix.
		 */
		Matrix4 inverse() const;
        
        /**
         * @brief	Creates a matrix from translation, rotation and scale. 
         * 			
		 * @note	The transformation are applied in scale->rotation->translation order.
         */
        void setTRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);

        /**
         * @brief	Creates a matrix from inverse translation, rotation and scale. 
         * 			
		 * @note	This is cheaper than "setTRS" and then performing "inverse".
         */
        void setInverseTRS(const Vector3& translation, const Quaternion& rotation, const Vector3& scale);

        /**
         * @brief	Decompose a Matrix4 to translation, rotation and scale.
         *
         * @note	Matrix must consist only of translation, rotation and uniform scale transformations,
         * 			otherwise accurate results are not guaranteed. Applying non-uniform scale guarantees
         * 			results will not be accurate.
         */
        void decomposition(Vector3& position, Quaternion& rotation, Vector3& scale) const;

        /**
		 * @brief	Check whether or not the matrix is affine matrix.
		 *
		 * @note	An affine matrix is a 4x4 matrix with row 3 equal to (0, 0, 0, 1),
		 *			i.e. no projective coefficients.
         */
        bool isAffine() const
        {
            return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
        }

        /**
         * @brief	Returns the inverse of the affine matrix.
         *
         * @note	Matrix must be affine.
         */
        Matrix4 inverseAffine() const;

        /**
         * @brief	Concatenate two affine matrices.
         *
         * @note	Both matrices must be affine.
         */
        Matrix4 concatenateAffine(const Matrix4 &other) const
        {
            assert(isAffine() && other.isAffine());

            return Matrix4(
                m[0][0] * other.m[0][0] + m[0][1] * other.m[1][0] + m[0][2] * other.m[2][0],
                m[0][0] * other.m[0][1] + m[0][1] * other.m[1][1] + m[0][2] * other.m[2][1],
                m[0][0] * other.m[0][2] + m[0][1] * other.m[1][2] + m[0][2] * other.m[2][2],
                m[0][0] * other.m[0][3] + m[0][1] * other.m[1][3] + m[0][2] * other.m[2][3] + m[0][3],

                m[1][0] * other.m[0][0] + m[1][1] * other.m[1][0] + m[1][2] * other.m[2][0],
                m[1][0] * other.m[0][1] + m[1][1] * other.m[1][1] + m[1][2] * other.m[2][1],
                m[1][0] * other.m[0][2] + m[1][1] * other.m[1][2] + m[1][2] * other.m[2][2],
                m[1][0] * other.m[0][3] + m[1][1] * other.m[1][3] + m[1][2] * other.m[2][3] + m[1][3],

                m[2][0] * other.m[0][0] + m[2][1] * other.m[1][0] + m[2][2] * other.m[2][0],
                m[2][0] * other.m[0][1] + m[2][1] * other.m[1][1] + m[2][2] * other.m[2][1],
                m[2][0] * other.m[0][2] + m[2][1] * other.m[1][2] + m[2][2] * other.m[2][2],
                m[2][0] * other.m[0][3] + m[2][1] * other.m[1][3] + m[2][2] * other.m[2][3] + m[2][3],

                0, 0, 0, 1);
        }

        /**
         * @brief	Transform a 3D vector by this matrix.
         * 			
         * @note	Matrix must be affine, if it is not use "multiply" method.
         */
        Vector3 multiply3x4(const Vector3& v) const
        {
            return Vector3(
                    m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
                    m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
                    m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
        }

        /**
         * @brief	Transform a 4D vector by this matrix.
         * 			
         * @note	Matrix must be affine, if it is not use "multiply" method.
         */
        Vector4 multiply3x4(const Vector4& v) const
        {
            return Vector4(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                v.w);
        }

        /**
         * @brief	Transform a plane by this matrix.
         * 			
         * @note	Matrix must be affine.
         */
        Plane multiply3x4(const Plane& p) const
        {
			Vector4 localNormal(p.normal.x, p.normal.y, p.normal.z, 0.0f);
			Vector4 localPoint = localNormal * p.d;
			localPoint.w = 1.0f;

			Matrix4 itMat = inverse().transpose();
			Vector4 worldNormal = itMat.multiply3x4(localNormal);
			Vector4 worldPoint = multiply3x4(localPoint);

			float d = worldNormal.dot(worldPoint);

			return Plane(worldNormal.x, worldNormal.y, worldNormal.z, d);
        }

        /**
         * @brief	Transform a 3D vector by this matrix.  
         *
         * @note	w component of the vector is assumed to be 1. After transformation all components
         * 			are projected back so that w remains 1.
         * 			
		 *			If your matrix doesn't contain projection components use "multiply3x4" method as it is faster.
         */
        Vector3 multiply(const Vector3 &v) const
        {
            Vector3 r;

            float fInvW = 1.0f / (m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3]);

            r.x = (m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3]) * fInvW;
            r.y = (m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3]) * fInvW;
            r.z = (m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]) * fInvW;

            return r;
        }

        /**
         * @brief	Transform a 4D vector by this matrix.  
         *
         * @note	After transformation all components are projected back so that w remains 1.
         * 			
		 *			If your matrix doesn't contain projection components use "multiply3x4" method as it is faster.
         */
        Vector4 multiply(const Vector4& v) const
        {
            return Vector4(
                m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w, 
                m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w,
                m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w,
                m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w
                );
        }

		/**
		 * @brief	Creates a view matrix and applies optional reflection.
		 */
		void makeView(const Vector3& position, const Quaternion& orientation, const Matrix4* reflectMatrix = nullptr);

		/**
		 * @brief	Creates an ortographic projection matrix.
		 */
		void makeProjectionOrtho(float left, float right, float top, float bottom, float near, float far);

		static const Matrix4 ZERO;
		static const Matrix4 IDENTITY;
    };

	BS_ALLOW_MEMCPY_SERIALIZATION(Matrix4);
}
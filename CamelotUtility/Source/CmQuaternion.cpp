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
// NOTE THAT THIS FILE IS BASED ON MATERIAL FROM:

// Geometric Tools, LLC
// Copyright (c) 1998-2010
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt

#include "CmQuaternion.h"

#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmVector3.h"

namespace CamelotFramework 
{
    const float Quaternion::EPSILON = 1e-03f;
    const Quaternion Quaternion::ZERO(0.0f, 0.0f, 0.0f, 0.0f);
    const Quaternion Quaternion::IDENTITY(1.0f, 0.0f, 0.0f, 0.0f);
	const Quaternion::EulerAngleOrderData Quaternion::EA_LOOKUP[6] = 
		{ { 0, 1, 2}, { 0, 2, 1}, { 1, 0, 2},
		{ 1, 2, 0}, { 2, 0, 1}, { 2, 1, 0} };;

    void Quaternion::fromRotationMatrix(const Matrix3& mat)
    {
        // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
        // article "Quaternion Calculus and Fast Animation".

        float trace = mat[0][0]+mat[1][1]+mat[2][2];
        float root;

        if (trace > 0.0f)
        {
            // |w| > 1/2, may as well choose w > 1/2
            root = Math::sqrt(trace + 1.0f);  // 2w
            w = 0.5f*root;
            root = 0.5f/root;  // 1/(4w)
            x = (mat[2][1]-mat[1][2])*root;
            y = (mat[0][2]-mat[2][0])*root;
            z = (mat[1][0]-mat[0][1])*root;
        }
        else
        {
            // |w| <= 1/2
            static size_t nextLookup[3] = { 1, 2, 0 };
            size_t i = 0;

            if (mat[1][1] > mat[0][0])
                i = 1;

            if (mat[2][2] > mat[i][i])
                i = 2;

            size_t j = nextLookup[i];
            size_t k = nextLookup[j];

            root = Math::sqrt(mat[i][i]-mat[j][j]-mat[k][k] + 1.0f);

            float* cmpntLookup[3] = { &x, &y, &z };
            *cmpntLookup[i] = 0.5f*root;
            root = 0.5f/root;

            w = (mat[k][j]-mat[j][k])*root;
            *cmpntLookup[j] = (mat[j][i]+mat[i][j])*root;
            *cmpntLookup[k] = (mat[k][i]+mat[i][k])*root;
        }

		normalize();
    }

    void Quaternion::fromAxisAngle(const Vector3& axis, const Radian& angle)
    {
        // Assert:  axis[] is unit length

        Radian fHalfAngle (0.5f*angle);
        float fSin = Math::sin(fHalfAngle);
        w = Math::cos(fHalfAngle);
        x = fSin*axis.x;
        y = fSin*axis.y;
        z = fSin*axis.z;
    }

    void Quaternion::fromAxes(const Vector3& xaxis, const Vector3& yaxis, const Vector3& zaxis)
    {
        Matrix3 kRot;

        kRot[0][0] = xaxis.x;
        kRot[1][0] = xaxis.y;
        kRot[2][0] = xaxis.z;

        kRot[0][1] = yaxis.x;
        kRot[1][1] = yaxis.y;
        kRot[2][1] = yaxis.z;

        kRot[0][2] = zaxis.x;
        kRot[1][2] = zaxis.y;
        kRot[2][2] = zaxis.z;

        fromRotationMatrix(kRot);
    }

	void Quaternion::fromEulerAngles(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle)
	{
		Quaternion quats[3];
		quats[0].fromAxisAngle(Vector3::UNIT_X, xAngle);
		quats[1].fromAxisAngle(Vector3::UNIT_Y, yAngle);
		quats[2].fromAxisAngle(Vector3::UNIT_Z, zAngle);

		*this = quats[2]*(quats[0] * quats[1]);
	}

	void Quaternion::fromEulerAngles(const Radian& xAngle, const Radian& yAngle, const Radian& zAngle, EulerAngleOrder order)
	{
		const EulerAngleOrderData& l = EA_LOOKUP[(int)order];

		Quaternion quats[3];
		quats[0].fromAxisAngle(Vector3::UNIT_X, xAngle);
		quats[1].fromAxisAngle(Vector3::UNIT_Y, yAngle);
		quats[2].fromAxisAngle(Vector3::UNIT_Z, zAngle);

		*this = quats[l.c]*(quats[l.a] * quats[l.b]);
	}

	void Quaternion::toRotationMatrix(Matrix3& mat) const
	{
		float fTx  = x+x;
		float fTy  = y+y;
		float fTz  = z+z;
		float fTwx = fTx*w;
		float fTwy = fTy*w;
		float fTwz = fTz*w;
		float fTxx = fTx*x;
		float fTxy = fTy*x;
		float fTxz = fTz*x;
		float fTyy = fTy*y;
		float fTyz = fTz*y;
		float fTzz = fTz*z;

		mat[0][0] = 1.0f-(fTyy+fTzz);
		mat[0][1] = fTxy-fTwz;
		mat[0][2] = fTxz+fTwy;
		mat[1][0] = fTxy+fTwz;
		mat[1][1] = 1.0f-(fTxx+fTzz);
		mat[1][2] = fTyz-fTwx;
		mat[2][0] = fTxz-fTwy;
		mat[2][1] = fTyz+fTwx;
		mat[2][2] = 1.0f-(fTxx+fTyy);
	}

	void Quaternion::toAxisAngle(Vector3& axis, Radian& angle) const
	{
		float fSqrLength = x*x+y*y+z*z;
		if ( fSqrLength > 0.0 )
		{
			angle = 2.0*Math::acos(w);
			float fInvLength = Math::invSqrt(fSqrLength);
			axis.x = x*fInvLength;
			axis.y = y*fInvLength;
			axis.z = z*fInvLength;
		}
		else
		{
			// Angle is 0 (mod 2*pi), so any axis will do
			angle = Radian(0.0);
			axis.x = 1.0;
			axis.y = 0.0;
			axis.z = 0.0;
		}
	}

	void Quaternion::toAxes(Vector3& xaxis, Vector3& yaxis, Vector3& zaxis) const
	{
		Matrix3 matRot;
		toRotationMatrix(matRot);

		xaxis.x = matRot[0][0];
		xaxis.y = matRot[1][0];
		xaxis.z = matRot[2][0];

		yaxis.x = matRot[0][1];
		yaxis.y = matRot[1][1];
		yaxis.z = matRot[2][1];

		zaxis.x = matRot[0][2];
		zaxis.y = matRot[1][2];
		zaxis.z = matRot[2][2];
	}

	bool Quaternion::toEulerAngles(Radian& xAngle, Radian& yAngle, Radian& zAngle) const
	{
		Matrix3 matRot;
		toRotationMatrix(matRot);
		return matRot.toEulerAngles(xAngle, yAngle, zAngle);
	}

	bool Quaternion::toEulerAngles(Radian& xAngle, Radian& yAngle, Radian& zAngle, EulerAngleOrder order) const
	{
		Matrix3 matRot;
		toRotationMatrix(matRot);
		return matRot.toEulerAngles(xAngle, yAngle, zAngle, order);
	}

    Vector3 Quaternion::xAxis() const
    {
        float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwy = fTy*w;
        float fTwz = fTz*w;
        float fTxy = fTy*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTzz = fTz*z;

        return Vector3(1.0f-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
    }

    Vector3 Quaternion::yAxis() const
    {
        float fTx  = 2.0f*x;
        float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwx = fTx*w;
        float fTwz = fTz*w;
        float fTxx = fTx*x;
        float fTxy = fTy*x;
        float fTyz = fTz*y;
        float fTzz = fTz*z;

        return Vector3(fTxy-fTwz, 1.0f-(fTxx+fTzz), fTyz+fTwx);
    }

    Vector3 Quaternion::zAxis() const
    {
        float fTx  = 2.0f*x;
        float fTy  = 2.0f*y;
        float fTz  = 2.0f*z;
        float fTwx = fTx*w;
        float fTwy = fTy*w;
        float fTxx = fTx*x;
        float fTxz = fTz*x;
        float fTyy = fTy*y;
        float fTyz = fTz*y;

        return Vector3(fTxz+fTwy, fTyz-fTwx, 1.0f-(fTxx+fTyy));
    }

    Quaternion Quaternion::operator+ (const Quaternion& rhs) const
    {
        return Quaternion(w+rhs.w,x+rhs.x,y+rhs.y,z+rhs.z);
    }

    Quaternion Quaternion::operator- (const Quaternion& rhs) const
    {
        return Quaternion(w-rhs.w,x-rhs.x,y-rhs.y,z-rhs.z);
    }

    Quaternion Quaternion::operator* (const Quaternion& rhs) const
    {
        return Quaternion
        (
            w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z,
            w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
            w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
            w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x
        );
    }

    Quaternion Quaternion::operator* (float rhs) const
    {
        return Quaternion(rhs*w,rhs*x,rhs*y,rhs*z);
    }

    Quaternion Quaternion::operator- () const
    {
        return Quaternion(-w,-x,-y,-z);
    }

    float Quaternion::dot(const Quaternion& other) const
    {
        return w*other.w+x*other.x+y*other.y+z*other.z;
    }

    Quaternion Quaternion::inverse() const
    {
        float fNorm = w*w+x*x+y*y+z*z;
        if (fNorm > 0.0f)
        {
            float fInvNorm = 1.0f/fNorm;
            return Quaternion(w*fInvNorm,-x*fInvNorm,-y*fInvNorm,-z*fInvNorm);
        }
        else
        {
            // Return an invalid result to flag the error
            return ZERO;
        }
    }

    Vector3 Quaternion::rotate(const Vector3& v) const
    {
		Matrix3 rot;
		toRotationMatrix(rot);
		return rot.transform(v);
    }

    Quaternion Quaternion::slerp(float t, const Quaternion& p, const Quaternion& q, bool shortestPath)
    {
        float cos = p.dot(q);
        Quaternion quat;

        if (cos < 0.0f && shortestPath)
        {
            cos = -cos;
            quat = -q;
        }
        else
        {
            quat = q;
        }

        if (Math::abs(cos) < 1 - EPSILON)
        {
            // Standard case (slerp)
            float sin = Math::sqrt(1 - Math::sqr(cos));
            Radian angle = Math::atan2(sin, cos);
            float invSin = 1.0f / sin;
            float coeff0 = Math::sin((1.0f - t) * angle) * invSin;
            float coeff1 = Math::sin(t * angle) * invSin;
            return coeff0 * p + coeff1 * quat;
        }
        else
        {
            // There are two situations:
            // 1. "p" and "q" are very close (fCos ~= +1), so we can do a linear
            //    interpolation safely.
            // 2. "p" and "q" are almost inverse of each other (fCos ~= -1), there
            //    are an infinite number of possibilities interpolation. but we haven't
            //    have method to fix this case, so just use linear interpolation here.
            Quaternion ret = (1.0f - t) * p + t * quat;

            // Taking the complement requires renormalization
            ret.normalize();
            return ret;
        }
    }

    float Quaternion::normalize()
    {
        float len = w*w+x*x+y*y+z*z;
        float factor = 1.0f / Math::sqrt(len);
        *this = *this * factor;
        return len;
    }

	Quaternion operator* (float lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs*rhs.w,lhs*rhs.x,lhs*rhs.y,
			lhs*rhs.z);
	}
}

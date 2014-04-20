#include "CmQuaternion.h"

#include "CmMath.h"
#include "CmMatrix3.h"
#include "CmVector3.h"

namespace BansheeEngine 
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
            static UINT32 nextLookup[3] = { 1, 2, 0 };
            UINT32 i = 0;

            if (mat[1][1] > mat[0][0])
                i = 1;

            if (mat[2][2] > mat[i][i])
                i = 2;

            UINT32 j = nextLookup[i];
            UINT32 k = nextLookup[j];

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

        Radian halfAngle (0.5f*angle);
        float sin = Math::sin(halfAngle);
        w = Math::cos(halfAngle);
        x = sin*axis.x;
        y = sin*axis.y;
        z = sin*axis.z;
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
		float tx  = x+x;
		float ty  = y+y;
		float fTz  = z+z;
		float twx = tx*w;
		float twy = ty*w;
		float twz = fTz*w;
		float txx = tx*x;
		float txy = ty*x;
		float txz = fTz*x;
		float tyy = ty*y;
		float tyz = fTz*y;
		float tzz = fTz*z;

		mat[0][0] = 1.0f-(tyy+tzz);
		mat[0][1] = txy-twz;
		mat[0][2] = txz+twy;
		mat[1][0] = txy+twz;
		mat[1][1] = 1.0f-(txx+tzz);
		mat[1][2] = tyz-twx;
		mat[2][0] = txz-twy;
		mat[2][1] = tyz+twx;
		mat[2][2] = 1.0f-(txx+tyy);
	}

	void Quaternion::toAxisAngle(Vector3& axis, Radian& angle) const
	{
		float sqrLength = x*x+y*y+z*z;
		if ( sqrLength > 0.0 )
		{
			angle = 2.0*Math::acos(w);
			float invLength = Math::invSqrt(sqrLength);
			axis.x = x*invLength;
			axis.y = y*invLength;
			axis.z = z*invLength;
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

	Quaternion Quaternion::getRotationFromTo(const Vector3& from, const Vector3& dest, const Vector3& fallbackAxis)
	{
		// Based on Stan Melax's article in Game Programming Gems
		Quaternion q;

		Vector3 v0 = from;
		Vector3 v1 = dest;
		v0.normalize();
		v1.normalize();

		float d = v0.dot(v1);

		// If dot == 1, vectors are the same
		if (d >= 1.0f)
			return Quaternion::IDENTITY;

		if (d < (1e-6f - 1.0f))
		{
			if (fallbackAxis != Vector3::ZERO)
			{
				// Rotate 180 degrees about the fallback axis
				q.fromAxisAngle(fallbackAxis, Radian(Math::PI));
			}
			else
			{
				// Generate an axis
				Vector3 axis = Vector3::UNIT_X.cross(from);
				if (axis.isZeroLength()) // Pick another if colinear
					axis = Vector3::UNIT_Y.cross(from);
				axis.normalize();
				q.fromAxisAngle(axis, Radian(Math::PI));
			}
		}
		else
		{
			float s = Math::sqrt( (1+d)*2 );
			float invs = 1 / s;

			Vector3 c = v0.cross(v1);

			q.x = c.x * invs;
			q.y = c.y * invs;
			q.z = c.z * invs;
			q.w = s * 0.5f;
			q.normalize();
		}

		return q;
	}

	Quaternion operator* (float lhs, const Quaternion& rhs)
	{
		return Quaternion(lhs*rhs.w,lhs*rhs.x,lhs*rhs.y,
			lhs*rhs.z);
	}
}

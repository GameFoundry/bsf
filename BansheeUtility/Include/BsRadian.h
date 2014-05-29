#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsDegree.h"

namespace BansheeEngine
{
	/**
	 * @brief	Wrapper class which indicates a given angle value is in Radians.
	 * 			
	 *	@note Radian values are interchangeable with Degree values, and conversions
	 *	will be done automatically between them.
	 */
	class BS_UTILITY_EXPORT Radian
	{
		float mRad;

	public:
		explicit Radian(float r = 0.0f) : mRad(r) {}
		Radian(const Degree& d);
		Radian& operator= (const float& f) { mRad = f; return *this; }
		Radian& operator= (const Radian& r) { mRad = r.mRad; return *this; }
		Radian& operator= (const Degree& d);

		float valueDegrees() const;
		float valueRadians() const { return mRad; }

        const Radian& operator+ () const { return *this; }
		Radian operator+ (const Radian& r) const { return Radian (mRad + r.mRad); }
		Radian operator+ (const Degree& d) const;
		Radian& operator+= (const Radian& r) { mRad += r.mRad; return *this; }
		Radian& operator+= (const Degree& d);
		Radian operator- () const { return Radian(-mRad); }
		Radian operator- (const Radian& r) const { return Radian (mRad - r.mRad); }
		Radian operator- (const Degree& d) const;
		Radian& operator-= (const Radian& r) { mRad -= r.mRad; return *this; }
		Radian& operator-= (const Degree& d);
		Radian operator* (float f) const { return Radian (mRad * f); }
        Radian operator* (const Radian& f) const { return Radian (mRad * f.mRad); }
		Radian& operator*= (float f) { mRad *= f; return *this; }
		Radian operator/ (float f) const { return Radian (mRad / f); }
		Radian& operator/= (float f) { mRad /= f; return *this; }

		friend Radian operator* (float lhs, const Radian& rhs) { return Radian(lhs * rhs.mRad); }
		friend Radian operator/ (float lhs, const Radian& rhs) { return Radian(lhs / rhs.mRad); }
		friend Radian operator+ (Radian& lhs, float rhs) { return Radian(lhs.mRad + rhs); }
		friend Radian operator+ (float lhs, const Radian& rhs) { return Radian(lhs + rhs.mRad); }
		friend Radian operator- (const Radian& lhs, float rhs) { return Radian(lhs.mRad - rhs); }
		friend Radian operator- (const float lhs, const Radian& rhs) { return Radian(lhs - rhs.mRad); }

		bool operator<  (const Radian& r) const { return mRad <  r.mRad; }
		bool operator<= (const Radian& r) const { return mRad <= r.mRad; }
		bool operator== (const Radian& r) const { return mRad == r.mRad; }
		bool operator!= (const Radian& r) const { return mRad != r.mRad; }
		bool operator>= (const Radian& r) const { return mRad >= r.mRad; }
		bool operator>  (const Radian& r) const { return mRad >  r.mRad; }
	};
}
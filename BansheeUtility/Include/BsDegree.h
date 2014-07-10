//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPrerequisitesUtil.h"
#include "BsRadian.h"

namespace BansheeEngine
{
	/**
	 * @brief	Wrapper class which indicates a given angle value is in Degrees.
	 * 			
	 *	@note	Degree values are interchangeable with Radian values, and conversions
	 *			will be done automatically between them.
	 */
	class BS_UTILITY_EXPORT Degree
	{
		float mDeg;

	public:
		explicit Degree(float d = 0.0f) : mDeg(d) {}
		Degree(const Radian& r);
		Degree& operator= (const float& f) { mDeg = f; return *this; }
		Degree& operator= (const Degree& d) { mDeg = d.mDeg; return *this; }
		Degree& operator= (const Radian& r);

		float valueDegrees() const { return mDeg; }
		float valueRadians() const;

		const Degree& operator+ () const { return *this; }
		Degree operator+ (const Degree& d) const { return Degree (mDeg + d.mDeg); }
		Degree operator+ (const Radian& r) const;
		Degree& operator+= (const Degree& d) { mDeg += d.mDeg; return *this; }
		Degree& operator+= (const Radian& r);
		Degree operator- () const { return Degree(-mDeg); }
		Degree operator- (const Degree& d) const { return Degree (mDeg - d.mDeg); }
		Degree operator- (const Radian& r) const;
		Degree& operator-= (const Degree& d) { mDeg -= d.mDeg; return *this; }
		Degree& operator-= (const Radian& r);
		Degree operator* (float f) const { return Degree (mDeg * f); }
        Degree operator* (const Degree& f) const { return Degree (mDeg * f.mDeg); }
		Degree& operator*= (float f) { mDeg *= f; return *this; }
		Degree operator/ (float f) const { return Degree (mDeg / f); }
		Degree& operator/= (float f) { mDeg /= f; return *this; }

		friend Degree operator* (float lhs, const Degree& rhs) { return Degree(lhs * rhs.mDeg); }
		friend Degree operator/ (float lhs, const Degree& rhs) { return Degree(lhs / rhs.mDeg); }
		friend Degree operator+ (Degree& lhs, float rhs) { return Degree(lhs.mDeg + rhs); }
		friend Degree operator+ (float lhs, const Degree& rhs) { return Degree(lhs + rhs.mDeg); }
		friend Degree operator- (const Degree& lhs, float rhs) { return Degree(lhs.mDeg - rhs); }
		friend Degree operator- (const float lhs, const Degree& rhs) { return Degree(lhs - rhs.mDeg); }

		bool operator<  (const Degree& d) const { return mDeg <  d.mDeg; }
		bool operator<= (const Degree& d) const { return mDeg <= d.mDeg; }
		bool operator== (const Degree& d) const { return mDeg == d.mDeg; }
		bool operator!= (const Degree& d) const { return mDeg != d.mDeg; }
		bool operator>= (const Degree& d) const { return mDeg >= d.mDeg; }
		bool operator>  (const Degree& d) const { return mDeg >  d.mDeg; }
	};
}
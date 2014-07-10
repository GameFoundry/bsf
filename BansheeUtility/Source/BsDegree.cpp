//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDegree.h"
#include "BsMath.h"

namespace BansheeEngine
{
	Degree::Degree(const Radian& r) 
		:mDeg(r.valueDegrees()) 
	{ }

	Degree& Degree::operator= (const Radian& r) 
	{ 
		mDeg = r.valueDegrees(); return *this; 
	}

	Degree Degree::operator+ (const Radian& r) const 
	{ 
		return Degree (mDeg + r.valueDegrees()); 
	}

	Degree& Degree::operator+= (const Radian& r) 
	{ 
		mDeg += r.valueDegrees(); return *this; 
	}

	Degree Degree::operator- (const Radian& r) const 
	{ 
		return Degree (mDeg - r.valueDegrees()); 
	}

	Degree& Degree::operator-= (const Radian& r) 
	{ 
		mDeg -= r.valueDegrees(); return *this; 
	}

	inline float Degree::valueRadians() const
	{
		return mDeg * Math::DEG2RAD;
	}
}
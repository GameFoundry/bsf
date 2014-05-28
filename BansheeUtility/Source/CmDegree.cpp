#include "CmDegree.h"
#include "CmMath.h"

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
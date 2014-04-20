#include "CmRadian.h"
#include "CmMath.h"

namespace BansheeEngine
{
	inline Radian::Radian (const Degree& d) : mRad(d.valueRadians()) 
	{ }

	inline Radian& Radian::operator= (const Degree& d) 
	{
		mRad = d.valueRadians(); 
		return *this;
	}

	inline Radian Radian::operator+ (const Degree& d) const 
	{
		return Radian (mRad + d.valueRadians());
	}

	inline Radian& Radian::operator+= (const Degree& d) 
	{
		mRad += d.valueRadians();
		return *this;
	}

	inline Radian Radian::operator- (const Degree& d) const 
	{
		return Radian (mRad - d.valueRadians());
	}

	inline Radian& Radian::operator-= (const Degree& d) 
	{
		mRad -= d.valueRadians();
		return *this;
	}

	inline float Radian::valueDegrees() const
	{
		return mRad * Math::RAD2DEG;
	}
}
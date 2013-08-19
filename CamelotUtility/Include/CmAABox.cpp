#include "CmAABox.h"

namespace CamelotFramework
{
	const AABox AABox::BOX_EMPTY;

	AABox::AABox() 
		:mMinimum(Vector3::ZERO), mMaximum(Vector3::UNIT_SCALE)
	{
		// Default to a null box 
		setMin(Vector3(-0.5f, -0.5f, -0.5f));
		setMax(Vector3(0.5f, 0.5f, 0.5f));
	}

	AABox::AABox(const AABox & rkBox)
		:mMinimum(Vector3::ZERO), mMaximum(Vector3::UNIT_SCALE)
	{
		setExtents( rkBox.mMinimum, rkBox.mMaximum );
	}

	AABox::AABox(const Vector3& min, const Vector3& max)
		:mMinimum(Vector3::ZERO), mMaximum(Vector3::UNIT_SCALE)
	{
		setExtents( min, max );
	}

	AABox& AABox::operator=(const AABox& rhs)
	{
		setExtents(rhs.mMinimum, rhs.mMaximum);

		return *this;
	}

	void AABox::setExtents( const Vector3& min, const Vector3& max )
	{
        assert( (min.x <= max.x && min.y <= max.y && min.z <= max.z) &&
            "The minimum corner of the box must be less than or equal to maximum corner" );

		mMinimum = min;
		mMaximum = max;
	}

	Vector3 AABox::getCorner(CornerEnum cornerToGet) const
	{
		switch(cornerToGet)
		{
		case FAR_LEFT_BOTTOM:
			return mMinimum;
		case FAR_LEFT_TOP:
			return Vector3(mMinimum.x, mMaximum.y, mMinimum.z);
		case FAR_RIGHT_TOP:
			return Vector3(mMaximum.x, mMaximum.y, mMinimum.z);
		case FAR_RIGHT_BOTTOM:
			return Vector3(mMaximum.x, mMinimum.y, mMinimum.z);
		case NEAR_RIGHT_BOTTOM:
			return Vector3(mMaximum.x, mMinimum.y, mMaximum.z);
		case NEAR_LEFT_BOTTOM:
			return Vector3(mMinimum.x, mMinimum.y, mMaximum.z);
		case NEAR_LEFT_TOP:
			return Vector3(mMinimum.x, mMaximum.y, mMaximum.z);
		case NEAR_RIGHT_TOP:
			return mMaximum;
		default:
			return Vector3();
		}
	}

	void AABox::merge( const AABox& rhs )
	{
		Vector3 min = mMinimum;
		Vector3 max = mMaximum;
		max.makeCeil(rhs.mMaximum);
		min.makeFloor(rhs.mMinimum);

		setExtents(min, max);
	}

	void AABox::merge( const Vector3& point )
	{
		mMaximum.makeCeil(point);
		mMinimum.makeFloor(point);
	}

	void AABox::transform( const Matrix4& matrix )
	{
		Vector3 oldMin, oldMax, currentCorner;

		// Getting the old values so that we can use the existing merge method.
		oldMin = mMinimum;
		oldMax = mMaximum;

		// We sequentially compute the corners in the following order :
		// 0, 6, 5, 1, 2, 4 ,7 , 3
		// This sequence allows us to only change one member at a time to get at all corners.

		// For each one, we transform it using the matrix
		// Which gives the resulting point and merge the resulting point.

		// First corner 
		// min min min
		currentCorner = oldMin;
		merge( matrix * currentCorner );

		// min,min,max
		currentCorner.z = oldMax.z;
		merge( matrix * currentCorner );

		// min max max
		currentCorner.y = oldMax.y;
		merge( matrix * currentCorner );

		// min max min
		currentCorner.z = oldMin.z;
		merge( matrix * currentCorner );

		// max max min
		currentCorner.x = oldMax.x;
		merge( matrix * currentCorner );

		// max max max
		currentCorner.z = oldMax.z;
		merge( matrix * currentCorner );

		// max min max
		currentCorner.y = oldMin.y;
		merge( matrix * currentCorner );

		// max min min
		currentCorner.z = oldMin.z;
		merge( matrix * currentCorner ); 
	}

	void AABox::transformAffine(const Matrix4& m)
	{
		assert(m.isAffine());

		Vector3 centre = getCenter();
		Vector3 halfSize = getHalfSize();

		Vector3 newCentre = m.transformAffine(centre);
		Vector3 newHalfSize(
			Math::Abs(m[0][0]) * halfSize.x + Math::Abs(m[0][1]) * halfSize.y + Math::Abs(m[0][2]) * halfSize.z, 
			Math::Abs(m[1][0]) * halfSize.x + Math::Abs(m[1][1]) * halfSize.y + Math::Abs(m[1][2]) * halfSize.z,
			Math::Abs(m[2][0]) * halfSize.x + Math::Abs(m[2][1]) * halfSize.y + Math::Abs(m[2][2]) * halfSize.z);

		setExtents(newCentre - newHalfSize, newCentre + newHalfSize);
	}

	bool AABox::intersects(const AABox& b2) const
	{
		// Use up to 6 separating planes
		if (mMaximum.x < b2.mMinimum.x)
			return false;
		if (mMaximum.y < b2.mMinimum.y)
			return false;
		if (mMaximum.z < b2.mMinimum.z)
			return false;

		if (mMinimum.x > b2.mMaximum.x)
			return false;
		if (mMinimum.y > b2.mMaximum.y)
			return false;
		if (mMinimum.z > b2.mMaximum.z)
			return false;

		// otherwise, must be intersecting
		return true;
	}

	AABox AABox::intersection(const AABox& b2) const
	{
		Vector3 intMin = mMinimum;
        Vector3 intMax = mMaximum;

        intMin.makeCeil(b2.getMin());
        intMax.makeFloor(b2.getMax());

        // Check intersection isn't null
        if (intMin.x < intMax.x &&
            intMin.y < intMax.y &&
            intMin.z < intMax.z)
        {
            return AABox(intMin, intMax);
        }

        return AABox();
	}

	float AABox::volume(void) const
	{
		Vector3 diff = mMaximum - mMinimum;
		return diff.x * diff.y * diff.z;
	}

	void AABox::scale(const Vector3& s)
	{
		// NB assumes centered on origin
		Vector3 min = mMinimum * s;
		Vector3 max = mMaximum * s;
		setExtents(min, max);
	}

	bool AABox::intersects(const Sphere& s) const
	{
		return Math::intersects(s, *this); 
	}

	bool AABox::intersects(const Plane& p) const
	{
		return Math::intersects(p, *this);
	}

	bool AABox::intersects(const Vector3& v) const
	{
		return(v.x >= mMinimum.x  &&  v.x <= mMaximum.x  && 
			v.y >= mMinimum.y  &&  v.y <= mMaximum.y  && 
			v.z >= mMinimum.z  &&  v.z <= mMaximum.z);
	}

	Vector3 AABox::getCenter(void) const
	{
		return Vector3(
			(mMaximum.x + mMinimum.x) * 0.5f,
			(mMaximum.y + mMinimum.y) * 0.5f,
			(mMaximum.z + mMinimum.z) * 0.5f);
	}

	Vector3 AABox::getSize(void) const
	{
		return mMaximum - mMinimum;
	}

	Vector3 AABox::getHalfSize(void) const
	{
		return (mMaximum - mMinimum) * 0.5;
	}

    bool AABox::contains(const Vector3& v) const
    {
        return mMinimum.x <= v.x && v.x <= mMaximum.x &&
                mMinimum.y <= v.y && v.y <= mMaximum.y &&
                mMinimum.z <= v.z && v.z <= mMaximum.z;
    }

    bool AABox::contains(const AABox& other) const
    {
        return this->mMinimum.x <= other.mMinimum.x &&
                this->mMinimum.y <= other.mMinimum.y &&
                this->mMinimum.z <= other.mMinimum.z &&
                other.mMaximum.x <= this->mMaximum.x &&
                other.mMaximum.y <= this->mMaximum.y &&
                other.mMaximum.z <= this->mMaximum.z;
    }

    bool AABox::operator== (const AABox& rhs) const
    {
        return this->mMinimum == rhs.mMinimum &&
                this->mMaximum == rhs.mMaximum;
    }

    bool AABox::operator!= (const AABox& rhs) const
    {
        return !(*this == rhs);
    }
}


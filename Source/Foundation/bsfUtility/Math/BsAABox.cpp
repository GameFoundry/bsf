//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Math/BsAABox.h"
#include "Math/BsRay.h"
#include "Math/BsPlane.h"
#include "Math/BsSphere.h"
#include "Math/BsMath.h"

namespace bs
{
	const AABox AABox::BOX_EMPTY = AABox(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
	const AABox AABox::UNIT_BOX = AABox(Vector3(-0.5f, -0.5f, -0.5f), Vector3(0.5f, 0.5f, 0.5f));
	const AABox AABox::INF_BOX = AABox(
		Vector3(
			std::numeric_limits<float>::infinity(), 
			std::numeric_limits<float>::infinity(), 
			std::numeric_limits<float>::infinity()), 
		Vector3(
			-std::numeric_limits<float>::infinity(), 
			-std::numeric_limits<float>::infinity(), 
			-std::numeric_limits<float>::infinity()));

	const UINT32 AABox::CUBE_INDICES[36] =
	{
		// Near
		NEAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_RIGHT_TOP,
		NEAR_LEFT_BOTTOM, NEAR_RIGHT_TOP, NEAR_RIGHT_BOTTOM,

		// Far
		FAR_RIGHT_BOTTOM, FAR_RIGHT_TOP, FAR_LEFT_TOP,
		FAR_RIGHT_BOTTOM, FAR_LEFT_TOP, FAR_LEFT_BOTTOM,

		// Left
		FAR_LEFT_BOTTOM, FAR_LEFT_TOP, NEAR_LEFT_TOP,
		FAR_LEFT_BOTTOM, NEAR_LEFT_TOP, NEAR_LEFT_BOTTOM,

		// Right
		NEAR_RIGHT_BOTTOM, NEAR_RIGHT_TOP, FAR_RIGHT_TOP,
		NEAR_RIGHT_BOTTOM, FAR_RIGHT_TOP, FAR_RIGHT_BOTTOM,

		// Top
		FAR_LEFT_TOP, FAR_RIGHT_TOP, NEAR_RIGHT_TOP,
		FAR_LEFT_TOP, NEAR_RIGHT_TOP, NEAR_LEFT_TOP,

		// Bottom
		NEAR_LEFT_BOTTOM, NEAR_RIGHT_BOTTOM, FAR_RIGHT_BOTTOM,
		NEAR_LEFT_BOTTOM, FAR_RIGHT_BOTTOM, FAR_LEFT_BOTTOM
	};

	AABox::AABox()
	{
		// Default to a unit box
		setMin(Vector3(-0.5f, -0.5f, -0.5f));
		setMax(Vector3(0.5f, 0.5f, 0.5f));
	}

	AABox::AABox(const Vector3& min, const Vector3& max)
	{
		setExtents(min, max);
	}

	void AABox::setExtents(const Vector3& min, const Vector3& max)
	{
		mMinimum = min;
		mMaximum = max;
	}

	void AABox::scale(const Vector3& s)
	{
		Vector3 center = getCenter();
		Vector3 min = center + (mMinimum - center) * s;
		Vector3 max = center + (mMaximum - center) * s;

		setExtents(min, max);
	}

	Vector3 AABox::getCorner(Corner cornerToGet) const
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
			return Vector3(BsZero);
		}
	}

	void AABox::merge(const AABox& rhs)
	{
		Vector3 min = mMinimum;
		Vector3 max = mMaximum;
		max.max(rhs.mMaximum);
		min.min(rhs.mMinimum);

		setExtents(min, max);
	}

	void AABox::merge(const Vector3& point)
	{
		mMaximum.max(point);
		mMinimum.min(point);
	}

	void AABox::transform(const Matrix4& matrix)
	{
		// Getting the old values so that we can use the existing merge method.
		Vector3 oldMin = mMinimum;
		Vector3 oldMax = mMaximum;

		Vector3 currentCorner;
		// We sequentially compute the corners in the following order :
		// 0, 6, 5, 1, 2, 4, 7, 3
		// This sequence allows us to only change one member at a time to get at all corners.

		// For each one, we transform it using the matrix
		// Which gives the resulting point and merge the resulting point.

		// First corner
		// min min min
		currentCorner = oldMin;
		merge(matrix.multiplyAffine(currentCorner));

		// min,min,max
		currentCorner.z = oldMax.z;
		merge(matrix.multiplyAffine(currentCorner));

		// min max max
		currentCorner.y = oldMax.y;
		merge(matrix.multiplyAffine(currentCorner));

		// min max min
		currentCorner.z = oldMin.z;
		merge(matrix.multiplyAffine(currentCorner));

		// max max min
		currentCorner.x = oldMax.x;
		merge(matrix.multiplyAffine(currentCorner));

		// max max max
		currentCorner.z = oldMax.z;
		merge(matrix.multiplyAffine(currentCorner));

		// max min max
		currentCorner.y = oldMin.y;
		merge(matrix.multiplyAffine(currentCorner));

		// max min min
		currentCorner.z = oldMin.z;
		merge(matrix.multiplyAffine(currentCorner));
	}

	void AABox::transformAffine(const Matrix4& m)
	{
		Vector3 min = m.getTranslation();
		Vector3 max = m.getTranslation();
		for(UINT32 i = 0; i < 3; i++)
		{
			for(UINT32 j = 0; j < 3; j++)
			{
				float e = m[i][j] * mMinimum[j];
				float f = m[i][j] * mMaximum[j];

				if(e < f)
				{
					min[i] += e;
					max[i] += f;
				}
				else
				{
					min[i] += f;
					max[i] += e;
				}
			}

		}

		setExtents(min, max);
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

		// Otherwise, must be intersecting
		return true;
	}

	bool AABox::intersects(const Sphere& sphere) const
	{
		// Use splitting planes
		const Vector3& center = sphere.getCenter();
		float radius = sphere.getRadius();
		const Vector3& min = getMin();
		const Vector3& max = getMax();

		// Arvo's algorithm
		float s, d = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (center[i] < min[i])
			{
				s = center[i] - min[i];
				d += s * s;
			}
			else if(center[i] > max[i])
			{
				s = center[i] - max[i];
				d += s * s;
			}
		}
		return d <= radius * radius;
	}

	bool AABox::intersects(const Plane& p) const
	{
		return (p.getSide(*this) == Plane::BOTH_SIDE);
	}

	std::pair<bool, float> AABox::intersects(const Ray& ray) const
	{
		float lowt = 0.0f;
		float t;
		bool hit = false;
		Vector3 hitpoint(BsZero);
		const Vector3& min = getMin();
		const Vector3& max = getMax();
		const Vector3& rayorig = ray.getOrigin();
		const Vector3& raydir = ray.getDirection();

		// Check origin inside first
		if ((rayorig.x > min.x && rayorig.y > min.y && rayorig.z > min.z) && (rayorig.x < max.x && rayorig.y < max.y && rayorig.z < max.z))
		{
			return std::pair<bool, float>(true, 0.0f);
		}

		// Check each face in turn, only check closest 3
		// Min x
		if (rayorig.x <= min.x && raydir.x > 0)
		{
			t = (min.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max x
		if (rayorig.x >= max.x && raydir.x < 0)
		{
			t = (max.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min y
		if (rayorig.y <= min.y && raydir.y > 0)
		{
			t = (min.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max y
		if (rayorig.y >= max.y && raydir.y < 0)
		{
			t = (max.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min z
		if (rayorig.z <= min.z && raydir.z > 0)
		{
			t = (min.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max z
		if (rayorig.z >= max.z && raydir.z < 0)
		{
			t = (max.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}

		return std::pair<bool, float>(hit, lowt);

	}

	bool AABox::intersects(const Ray& ray, float& d1, float& d2) const
	{
		const Vector3& min = getMin();
		const Vector3& max = getMax();
		const Vector3& rayorig = ray.getOrigin();
		const Vector3& raydir = ray.getDirection();

		Vector3 absDir;
		absDir[0] = Math::abs(raydir[0]);
		absDir[1] = Math::abs(raydir[1]);
		absDir[2] = Math::abs(raydir[2]);

		// Sort the axis, ensure check minimise floating error axis first
		int imax = 0, imid = 1, imin = 2;
		if (absDir[0] < absDir[2])
		{
			imax = 2;
			imin = 0;
		}
		if (absDir[1] < absDir[imin])
		{
			imid = imin;
			imin = 1;
		}
		else if (absDir[1] > absDir[imax])
		{
			imid = imax;
			imax = 1;
		}

		float start = 0, end = Math::POS_INFINITY;

#define _CALC_AXIS(i)                                       \
	do {                                                    \
	float denom = 1 / raydir[i];                         \
	float newstart = (min[i] - rayorig[i]) * denom;      \
	float newend = (max[i] - rayorig[i]) * denom;        \
	if (newstart > newend) std::swap(newstart, newend); \
	if (newstart > end || newend < start) return false; \
	if (newstart > start) start = newstart;             \
	if (newend < end) end = newend;                     \
	} while(0)

		// Check each axis in turn

		_CALC_AXIS(imax);

		if (absDir[imid] < std::numeric_limits<float>::epsilon())
		{
			// Parallel with middle and minimise axis, check bounds only
			if (rayorig[imid] < min[imid] || rayorig[imid] > max[imid] ||
				rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
				return false;
		}
		else
		{
			_CALC_AXIS(imid);

			if (absDir[imin] < std::numeric_limits<float>::epsilon())
			{
				// Parallel with minimise axis, check bounds only
				if (rayorig[imin] < min[imin] || rayorig[imin] > max[imin])
					return false;
			}
			else
			{
				_CALC_AXIS(imin);
			}
		}
#undef _CALC_AXIS

		d1 = start;
		d2 = end;

		return true;
	}

	Vector3 AABox::getCenter() const
	{
		return Vector3(
			(mMaximum.x + mMinimum.x) * 0.5f,
			(mMaximum.y + mMinimum.y) * 0.5f,
			(mMaximum.z + mMinimum.z) * 0.5f);
	}

	Vector3 AABox::getSize() const
	{
		return mMaximum - mMinimum;
	}

	Vector3 AABox::getHalfSize() const
	{
		return (mMaximum - mMinimum) * 0.5;
	}

	float AABox::getRadius() const
	{
		return ((mMaximum - mMinimum) * 0.5).length();
	}

	float AABox::getVolume() const
	{
		Vector3 diff = mMaximum - mMinimum;
		return diff.x * diff.y * diff.z;
	}

	bool AABox::contains(const Vector3& v) const
	{
		return mMinimum.x <= v.x && v.x <= mMaximum.x &&
				mMinimum.y <= v.y && v.y <= mMaximum.y &&
				mMinimum.z <= v.z && v.z <= mMaximum.z;
	}

	bool AABox::contains(const Vector3& v, float extra) const
	{
		return (mMinimum.x - extra) <= v.x && v.x <= (mMaximum.x + extra) &&
			   (mMinimum.y - extra) <= v.y && v.y <= (mMaximum.y + extra) &&
			   (mMinimum.z - extra) <= v.z && v.z <= (mMaximum.z + extra);
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


#include "CmORect.h"

namespace CamelotFramework
{
	ORect::ORect()
	{	}

	ORect::ORect(const Rect& rect)
	{
		mOrigin = Vector2((float)rect.x, (float)rect.y);
		mSides[0] = Vector2((float)rect.width, 0.0f);
		mSides[1] = Vector2(0.0f, (float)rect.height);

		mSideLengths[0] = mSides[0].normalize();
		mSideLengths[1] = mSides[1].normalize();
	}

	void ORect::applyTransform(const Matrix4& tfrm)
	{
		Vector3 oldOrigin = Vector3(mOrigin.x, mOrigin.y, 0);

		Vector3 oldCornerA = oldOrigin + Vector3(mSides[0].x, mSides[0].y, 0.0f);
		Vector3 newCornerA = tfrm * oldCornerA;

		Vector3 oldCornerB = oldOrigin + Vector3(mSides[1].x, mSides[1].y, 0.0f);
		Vector3 newCornerB = tfrm * oldCornerB;

		Vector3 newOrigin = tfrm * oldOrigin;
		mOrigin.x = newOrigin.x;
		mOrigin.y = newOrigin.y;

		mSides[0].x = newCornerA.x - newOrigin.x;
		mSides[0].y = newCornerA.y - newOrigin.y;

		mSides[1].x = newCornerB.x - newOrigin.x;
		mSides[1].y = newCornerB.y - newOrigin.y;

		mSideLengths[0] = mSides[0].normalize();
		mSideLengths[1] = mSides[1].normalize();
	}

	bool ORect::contains(const Vector2& point)
	{
		Vector2 localPoint = point - mOrigin;

		float t1 = localPoint.x * mSides[0].x + localPoint.y * mSides[0].y;
		float t2 = localPoint.x * mSides[1].x + localPoint.y * mSides[1].y;

		if(t1 >= 0.0f && t1 <= mSideLengths[0] && t2 >= 0.0f && t1 <= mSideLengths[1])
			return true;

		return false;
	}

	Vector2 ORect::getMin() const
	{
		Vector2 result;

		result.x = std::min(std::min(mOrigin.x, mOrigin.x + mSides[0].x), mOrigin.x + mSides[1].x);
		result.y = std::min(std::min(mOrigin.y, mOrigin.y + mSides[0].y), mOrigin.y + mSides[1].y);

		return result;
	}

	Vector2 ORect::getMax() const
	{
		Vector2 result;

		result.x = std::max(std::max(mOrigin.x, mOrigin.x + mSides[0].x), mOrigin.x + mSides[1].x);
		result.y = std::max(std::max(mOrigin.y, mOrigin.y + mSides[0].y), mOrigin.y + mSides[1].y);

		return result;
	}
}
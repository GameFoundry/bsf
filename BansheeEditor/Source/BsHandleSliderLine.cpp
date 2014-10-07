#include "BsHandleSliderLine.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
#include "BsCapsule.h"
#include "BsLineSegment3.h"
#include "BsSphere.h"
#include "BsRay.h"

namespace BansheeEngine
{
	const float HandleSliderLine::CAPSULE_RADIUS = 0.2f;
	const float HandleSliderLine::SPHERE_RADIUS = 0.5f;

	HandleSliderLine::HandleSliderLine(const Vector3& direction, float length, float snapValue, bool fixedScale)
		:HandleSlider(fixedScale, snapValue), mLength(length), mDelta(0.0f)
	{
		mDirection = Vector3::normalize(direction);

		Vector3 start = Vector3::ZERO;
		Vector3 end = start + mDirection * length;

		Vector3 sphereCenter = start + mDirection * std::max(0.0f, length - SPHERE_RADIUS * 2);

		mCapsuleCollider = Capsule(LineSegment3(start, end), CAPSULE_RADIUS);
		mSphereCollider = Sphere(sphereCenter, SPHERE_RADIUS);

		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._registerSlider(this);
	}

	HandleSliderLine::~HandleSliderLine()
	{
		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._unregisterSlider(this);
	}

	bool HandleSliderLine::intersects(const Ray& ray, float& t) const
	{
		Ray localRay = ray;
		localRay.transform(getTransformInv());

		auto capsuleIntersect = mCapsuleCollider.intersects(ray);
		auto sphereIntersect = mSphereCollider.intersects(ray);

		t = std::numeric_limits<float>::max();
		bool gotIntersect = false;

		if (capsuleIntersect.first)
		{
			t = capsuleIntersect.second;
			gotIntersect = true;
		}

		if (sphereIntersect.first)
		{
			if (sphereIntersect.second < t)
			{
				t = sphereIntersect.second;
				gotIntersect = true;
			}
		}

		return gotIntersect;
	}

	void HandleSliderLine::update(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray)
	{
		assert(getState() == State::Active);

		mLastPointerPos = mCurPointerPos;
		mCurPointerPos = pointerPos;

		mDelta = calcDelta(camera, getPosition(), mDirection, mLastPointerPos, mCurPointerPos);
	}

	void HandleSliderLine::reset()
	{
		mDelta = 0.0f;
	}

	Vector3 HandleSliderLine::getNewPosition() const
	{
		return getPosition() + mDirection * mDelta;
	}
}
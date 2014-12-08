#include "BsHandleSliderPlane.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
#include "BsVector3.h"
#include "BsRay.h"

namespace BansheeEngine
{
	HandleSliderPlane::HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, bool fixedScale)
		:HandleSlider(fixedScale), mLength(length)
	{
		mDirection1 = Vector3::normalize(dir1);
		mDirection2 = Vector3::normalize(dir2);

		std::array<Vector3, 2> axes = { mDirection1, mDirection2 };
		std::array<float, 2> extents = { length, length };

		mCollider = Rect3(Vector3::ZERO, axes, extents);

		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._registerSlider(this);
	}

	HandleSliderPlane::~HandleSliderPlane()
	{
		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._unregisterSlider(this);
	}


	bool HandleSliderPlane::intersects(const Ray& ray, float& t) const
	{
		Ray localRay = ray;
		localRay.transform(getTransformInv());

		auto intersect = mCollider.intersects(localRay);

		if (intersect.first)
		{
			t = intersect.second;

			return true;
		}

		return false;
	}

	void HandleSliderPlane::handleInput(const CameraHandlerPtr& camera, const Vector2I& inputDelta)
	{
		assert(getState() == State::Active);

		mCurrentPointerPos += inputDelta;
		mDelta.x = calcDelta(camera, mStartPosition, getRotation().rotate(mDirection1), mStartPointerPos, mCurrentPointerPos);
		mDelta.y = calcDelta(camera, mStartPosition, getRotation().rotate(mDirection2), mStartPointerPos, mCurrentPointerPos);
	}
}
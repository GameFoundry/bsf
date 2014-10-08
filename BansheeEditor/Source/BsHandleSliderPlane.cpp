#include "BsHandleSliderPlane.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
#include "BsVector3.h"
#include "BsRay.h"

namespace BansheeEngine
{
	HandleSliderPlane::HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, float snapValue, bool fixedScale)
		:HandleSlider(fixedScale, snapValue), mLength(length)
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

		auto intersect = mCollider.intersects(ray);

		if (intersect.first)
		{
			t = intersect.second;

			return true;
		}

		return false;
	}

	void HandleSliderPlane::reset()
	{
		// TODO - Clear delta
	}

	void HandleSliderPlane::update(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray)
	{
		assert(getState() == State::Active);

		mLastPointerPos = mCurPointerPos;
		mCurPointerPos = pointerPos;

		// TODO
	}
}
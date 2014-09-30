#include "BsHandleSliderPlane.h"
#include "BsHandleManager.h"
#include "BsRect3.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	HandleSliderPlane::HandleSliderPlane(const Vector3& dir1, const Vector3& dir2, float length, float snapValue, bool fixedScale)
		:HandleSlider(fixedScale, snapValue), mLength(length)
	{
		mDirection1 = Vector3::normalize(dir1);
		mDirection2 = Vector3::normalize(dir2);

		std::array<Vector3, 2> axes = { mDirection1, mDirection2 };
		std::array<float, 2> extents = { length, length };

		Rect3 collider(Vector3::ZERO, axes, extents);
		HandleManager::instance()._registerRectCollider(collider, this);
	}

	HandleSliderPlane::~HandleSliderPlane()
	{
		HandleManager::instance()._unregisterSlider(this);
	}

	Vector3 HandleSliderPlane::updateDelta(const Vector3& oldValue) const
	{
		return oldValue;

		// TODO - Don't  forget to consider currently active transform (and also custom handle transform)
		// - Both position and direction need to consider it
	}
}
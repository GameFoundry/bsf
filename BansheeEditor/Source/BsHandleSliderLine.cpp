#include "BsHandleSliderLine.h"
#include "BsHandleManager.h"
#include "BsCapsule.h"
#include "BsLineSegment3.h"
#include "BsSphere.h"

namespace BansheeEngine
{
	const float HandleSliderLine::CAPSULE_RADIUS = 0.2f;
	const float HandleSliderLine::SPHERE_RADIUS = 0.5f;

	HandleSliderLine::HandleSliderLine(const Vector3& direction, float length, float snapValue, bool fixedScale)
		:HandleSlider(fixedScale, snapValue), mLength(length)
	{
		mDirection = Vector3::normalize(direction);

		Vector3 start = Vector3::ZERO;
		Vector3 end = start + mDirection * length;

		Vector3 sphereCenter = start + mDirection * std::max(0.0f, length - SPHERE_RADIUS * 2);

		HandleManager::instance()._registerCapsuleCollider(Capsule(LineSegment3(start, end), CAPSULE_RADIUS), this);
		HandleManager::instance()._registerSphereCollider(Sphere(sphereCenter, SPHERE_RADIUS), this);
	}

	HandleSliderLine::~HandleSliderLine()
	{
		HandleManager::instance()._unregisterSlider(this);
	}

	Vector3 HandleSliderLine::updateDelta(const Vector3& oldValue) const
	{
		return oldValue;

		// TODO - Don't  forget to consider currently active transform (and also custom handle transform)
		// - Both position and direction need to consider it
	}
}
#include "BsHandleSliderDisc.h"
#include "BsHandleManager.h"
#include "BsTorus.h"
#include "BsVector3.h"
#include "BsQuaternion.h"

namespace BansheeEngine
{
	const float HandleSliderDisc::TORUS_RADIUS = 0.5f;

	HandleSliderDisc::HandleSliderDisc(const Vector3& normal, float radius, float snapValue, bool fixedScale)
		:HandleSlider(fixedScale, snapValue), mRadius(radius)
	{
		Vector3 x, z;
		mNormal.orthogonalComplement(x, z);

		mTorusRotation = (Matrix4)Matrix3(x, mNormal, z); // Our Torus class doesn't allow us to specify a normal so we embed it here

		Torus collider(radius, TORUS_RADIUS);
		HandleManager::instance()._registerTorusCollider(collider, this);
	}

	HandleSliderDisc::~HandleSliderDisc()
	{
		HandleManager::instance()._unregisterSlider(this);
	}

	Quaternion HandleSliderDisc::updateDelta(const Quaternion& oldValue) const
	{
		return oldValue;

		// TODO - Don't  forget to consider currently active transform (and also custom handle transform)
		// - Both position and direction need to consider it
	}

	void HandleSliderDisc::setCustomTransform(const Matrix4& transform)
	{
		HandleSlider::setCustomTransform(transform * mTorusRotation);
	}
}
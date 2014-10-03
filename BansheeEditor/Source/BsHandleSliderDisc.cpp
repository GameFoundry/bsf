#include "BsHandleSliderDisc.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
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

		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._registerTorusCollider(collider, this);
	}

	HandleSliderDisc::~HandleSliderDisc()
	{
		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._unregisterSlider(this);
	}

	Quaternion HandleSliderDisc::updateDelta(const Quaternion& oldValue) const
	{
		return oldValue;

		// TODO - Don't  forget to consider currently active transform (and also custom handle transform)
		// - Both position and direction need to consider it
	}

	const Matrix4& HandleSliderDisc::getTransform() const
	{
		return mTransform * mTorusRotation;
	}
}
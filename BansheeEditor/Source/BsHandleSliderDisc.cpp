#include "BsHandleSliderDisc.h"
#include "BsHandleManager.h"
#include "BsHandleSliderManager.h"
#include "BsRay.h"
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

		mCollider = Torus(radius, TORUS_RADIUS);

		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._registerSlider(this);
	}

	HandleSliderDisc::~HandleSliderDisc()
	{
		HandleSliderManager& sliderManager = HandleManager::instance().getSliderManager();
		sliderManager._unregisterSlider(this);
	}

	void HandleSliderDisc::updateCachedTransform() const
	{
		mTransform.setTRS(mPosition, mRotation, mScale);
		mTransform = mTransform * mTorusRotation;
		mTransformInv = mTransform.inverseAffine();
		mTransformDirty = false;
	}

	bool HandleSliderDisc::intersects(const Ray& ray, float& t) const
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

	void HandleSliderDisc::reset()
	{
		// TODO - Clear delta
	}

	void HandleSliderDisc::update(const HCamera& camera, const Vector2I& pointerPos, const Ray& ray)
	{
		assert(getState() == State::Active);

		mLastPointerPos = mCurPointerPos;
		mCurPointerPos = pointerPos;

		// TODO
	}
}
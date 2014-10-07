#include "BsHandleSlider.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	HandleSlider::HandleSlider(bool fixedScale, float snapValue)
		:mFixedScale(fixedScale), mSnapValue(snapValue), mScale(Vector3::ONE), mTransformDirty(true)
	{

	}

	void HandleSlider::setPosition(const Vector3& position)
	{
		mPosition = position;
		mTransformDirty = true;
	}

	void HandleSlider::setRotation(const Quaternion& rotation)
	{
		mRotation = rotation;
		mTransformDirty = true;
	}

	void HandleSlider::setScale(const Vector3& scale)
	{
		mScale = scale;
		mTransformDirty = true;
	}

	const Matrix4& HandleSlider::getTransform() const
	{
		if (mTransformDirty)
			updateCachedTransform();

		return mTransform;
	}

	const Matrix4& HandleSlider::getTransformInv() const
	{
		if (mTransformDirty)
			updateCachedTransform();

		return mTransformInv;
	}

	void HandleSlider::updateCachedTransform() const
	{
		mTransform.setTRS(mPosition, mRotation, mScale);
		mTransformInv = mTransform.inverseAffine();
		mTransformDirty = false;
	}

	float HandleSlider::calcDelta(const HCamera& camera, const Vector3& position, const Vector3& direction,
		const Vector2I& pointerStart, const Vector2I& pointerEnd)
	{
		Vector2I handleStart2D = camera->worldToScreenPoint(position);
		Vector2I handleEnd2D = camera->worldToScreenPoint(position + direction);

		Vector2I handleDir2D = handleEnd2D - handleStart2D;
		INT32 sqrdMag = handleDir2D.squaredLength();

		if (sqrdMag == 0)
			return 0.0f;

		Vector2I diffStart = pointerStart - handleStart2D;
		Vector2I diffEnd = pointerEnd - handleStart2D;

		float tStart = handleDir2D.dot(diffStart) / (float)sqrdMag;
		float tEnd = handleDir2D.dot(diffEnd) / (float)sqrdMag;

		return tEnd - tStart;
	}
}
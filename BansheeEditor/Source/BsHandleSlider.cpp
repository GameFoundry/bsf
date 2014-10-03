#include "BsHandleSlider.h"

namespace BansheeEngine
{
	HandleSlider::HandleSlider(bool fixedScale, float snapValue)
		:mFixedScale(fixedScale), mSnapValue(snapValue), mScale(Vector3::ONE)
	{
		mTransform.setTRS(mPosition, mRotation, mScale);
	}

	void HandleSlider::setPosition(const Vector3& position)
	{
		mPosition = position;
		mTransform.setTRS(mPosition, mRotation, mScale);
	}

	void HandleSlider::setRotation(const Quaternion& rotation)
	{
		mRotation = rotation;
		mTransform.setTRS(mPosition, mRotation, mScale);
	}

	void HandleSlider::setScale(const Vector3& scale)
	{
		mScale = scale;
		mTransform.setTRS(mPosition, mRotation, mScale);
	}

	void HandleSlider::registerDrag(const Vector2I& pointerPos)
	{
		assert(getState() == State::Active);

		mLastPointerPos = mCurPointerPos;
		mCurPointerPos = pointerPos;
	}
}
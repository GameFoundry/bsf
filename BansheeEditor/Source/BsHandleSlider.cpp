#include "BsHandleSlider.h"

namespace BansheeEngine
{
	HandleSlider::HandleSlider(bool fixedScale, float snapValue)
		:mFixedScale(fixedScale), mSnapValue(snapValue)
	{

	}

	void HandleSlider::registerDrag(const Vector2I& pointerPos)
	{
		assert(getState() == State::Active);

		mLastPointerPos = mCurPointerPos;
		mCurPointerPos = pointerPos;
	}
}
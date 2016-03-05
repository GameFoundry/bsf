//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCSliderJoint.h"
#include "BsSceneObject.h"
#include "BsCSliderJointRTTI.h"

namespace BansheeEngine
{
	CSliderJoint::CSliderJoint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("SliderJoint");
	}

	float CSliderJoint::getPosition() const
	{
		if (mInternal == nullptr)
			return 0.0f;

		return _getInternal()->getPosition();
	}

	float CSliderJoint::getSpeed() const
	{
		if (mInternal == nullptr)
			return 0.0f;

		return _getInternal()->getSpeed();
	}

	LimitLinearRange CSliderJoint::getLimit() const
	{
		return mLimit;
	}

	void CSliderJoint::setLimit(const LimitLinearRange& limit)
	{
		if (mLimit == limit)
			return;

		mLimit = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimit(limit);
	}

	void CSliderJoint::setFlag(SliderJoint::Flag flag, bool enabled)
	{
		bool isEnabled = ((UINT32)mFlag & (UINT32)flag) != 0;
		if (isEnabled == enabled)
			return;

		if (enabled)
			mFlag = (SliderJoint::Flag)((UINT32)mFlag | (UINT32)flag);
		else
			mFlag = (SliderJoint::Flag)((UINT32)mFlag & ~(UINT32)flag);

		if (mInternal != nullptr)
			_getInternal()->setFlag(flag, enabled);
	}

	bool CSliderJoint::hasFlag(SliderJoint::Flag flag) const
	{
		return ((UINT32)mFlag & (UINT32)flag) != 0;
	}

	SPtr<Joint> CSliderJoint::createInternal()
	{
		SPtr<Joint> joint = SliderJoint::create();

		joint->_setOwner(PhysicsOwnerType::Component, this);
		return joint;
	}

	RTTITypeBase* CSliderJoint::getRTTIStatic()
	{
		return CSliderJointRTTI::instance();
	}

	RTTITypeBase* CSliderJoint::getRTTI() const
	{
		return CSliderJoint::getRTTIStatic();
	}
}
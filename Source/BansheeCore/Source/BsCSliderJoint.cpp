//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCSliderJoint.h"
#include "BsSceneObject.h"
#include "BsCSliderJointRTTI.h"

namespace BansheeEngine
{
	CSliderJoint::CSliderJoint()
		: CJoint(mDesc)
	{ }

	CSliderJoint::CSliderJoint(const HSceneObject& parent)
		: CJoint(parent, mDesc)
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
		return mDesc.limit;
	}

	void CSliderJoint::setLimit(const LimitLinearRange& limit)
	{
		if (mDesc.limit == limit)
			return;

		mDesc.limit = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimit(limit);
	}

	void CSliderJoint::setFlag(SliderJoint::Flag flag, bool enabled)
	{
		bool isEnabled = ((UINT32)mDesc.flag & (UINT32)flag) != 0;
		if (isEnabled == enabled)
			return;

		if (enabled)
			mDesc.flag = (SliderJoint::Flag)((UINT32)mDesc.flag | (UINT32)flag);
		else
			mDesc.flag = (SliderJoint::Flag)((UINT32)mDesc.flag & ~(UINT32)flag);

		if (mInternal != nullptr)
			_getInternal()->setFlag(flag, enabled);
	}

	bool CSliderJoint::hasFlag(SliderJoint::Flag flag) const
	{
		return ((UINT32)mDesc.flag & (UINT32)flag) != 0;
	}

	SPtr<Joint> CSliderJoint::createInternal()
	{
		SPtr<Joint> joint = SliderJoint::create(mDesc);

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
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCHingeJoint.h"
#include "BsSceneObject.h"
#include "BsCHingeJointRTTI.h"

namespace BansheeEngine
{
	CHingeJoint::CHingeJoint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("HingeJoint");
	}

	Radian CHingeJoint::getAngle() const
	{
		if (mInternal == nullptr)
			return Radian(0.0f);

		return _getInternal()->getAngle();
	}

	float CHingeJoint::getSpeed() const
	{
		if (mInternal == nullptr)
			return 0.0f;

		return _getInternal()->getSpeed();
	}

	LimitAngularRange CHingeJoint::getLimit() const
	{
		return mLimit;
	}

	void CHingeJoint::setLimit(const LimitAngularRange& limit)
	{
		if (limit == mLimit)
			return;

		mLimit = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimit(limit);
	}

	HingeJoint::Drive CHingeJoint::getDrive() const
	{
		return mDrive;
	}

	void CHingeJoint::setDrive(const HingeJoint::Drive& drive)
	{
		if (drive == mDrive)
			return;

		mDrive = drive;

		if (mInternal != nullptr)
			_getInternal()->setDrive(drive);
	}

	void CHingeJoint::setFlag(HingeJoint::Flag flag, bool enabled)
	{
		bool isEnabled = ((UINT32)mFlag & (UINT32)flag) != 0;
		if (isEnabled == enabled)
			return;

		if (enabled)
			mFlag = (HingeJoint::Flag)((UINT32)mFlag | (UINT32)flag);
		else
			mFlag = (HingeJoint::Flag)((UINT32)mFlag & ~(UINT32)flag);

		if (mInternal != nullptr)
			_getInternal()->setFlag(flag, enabled);
	}

	bool CHingeJoint::hasFlag(HingeJoint::Flag flag) const
	{
		return ((UINT32)mFlag & (UINT32)flag) != 0;
	}

	SPtr<Joint> CHingeJoint::createInternal()
	{
		return HingeJoint::create();
	}

	RTTITypeBase* CHingeJoint::getRTTIStatic()
	{
		return CHingeJointRTTI::instance();
	}

	RTTITypeBase* CHingeJoint::getRTTI() const
	{
		return CHingeJoint::getRTTIStatic();
	}
}
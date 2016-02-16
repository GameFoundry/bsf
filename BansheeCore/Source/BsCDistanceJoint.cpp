//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCDistanceJoint.h"
#include "BsSceneObject.h"
#include "BsCDistanceJointRTTI.h"

namespace BansheeEngine
{
	CDistanceJoint::CDistanceJoint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("DistanceJoint");
	}

	float CDistanceJoint::getDistance() const
	{
		if (mInternal == nullptr)
			return 0.0f;

		return _getInternal()->getDistance();
	}

	float CDistanceJoint::getMinDistance() const
	{
		return mMinDistance;
	}

	void CDistanceJoint::setMinDistance(float value)
	{
		if (mMinDistance == value)
			return;

		mMinDistance = value;

		if (mInternal != nullptr)
			_getInternal()->setMinDistance(value);
	}

	float CDistanceJoint::getMaxDistance() const
	{
		return mMaxDistance;
	}

	void CDistanceJoint::setMaxDistance(float value)
	{
		if (mMaxDistance == value)
			return;

		mMaxDistance = value;

		if (mInternal != nullptr)
			_getInternal()->setMaxDistance(value);
	}

	float CDistanceJoint::getTolerance() const
	{
		return mTolerance;
	}

	void CDistanceJoint::setTolerance(float value)
	{
		if (mTolerance == value)
			return;

		mTolerance = value;

		if (mInternal != nullptr)
			_getInternal()->setTolerance(value);
	}

	Spring CDistanceJoint::getSpring() const
	{
		return mSpring;
	}

	void CDistanceJoint::setSpring(const Spring& value)
	{
		if (mSpring == value)
			return;

		mSpring = value;

		if(mInternal != nullptr)
			_getInternal()->setSpring(value);
	}

	void CDistanceJoint::setFlag(DistanceJoint::Flag flag, bool enabled)
	{
		bool isEnabled = ((UINT32)mFlag & (UINT32)flag) != 0;
		if (isEnabled == enabled)
			return;

		if (enabled)
			mFlag = (DistanceJoint::Flag)((UINT32)mFlag | (UINT32)flag);
		else
			mFlag = (DistanceJoint::Flag)((UINT32)mFlag & ~(UINT32)flag);

		if (mInternal != nullptr)
			_getInternal()->setFlag(flag, enabled);
	}

	bool CDistanceJoint::hasFlag(DistanceJoint::Flag flag) const
	{
		return ((UINT32)mFlag & (UINT32)flag) != 0;
	}

	SPtr<Joint> CDistanceJoint::createInternal()
	{
		return DistanceJoint::create();
	}

	RTTITypeBase* CDistanceJoint::getRTTIStatic()
	{
		return CDistanceJointRTTI::instance();
	}

	RTTITypeBase* CDistanceJoint::getRTTI() const
	{
		return CDistanceJoint::getRTTIStatic();
	}
}
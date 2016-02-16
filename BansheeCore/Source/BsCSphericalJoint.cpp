//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCSphericalJoint.h"
#include "BsSceneObject.h"
#include "BsCSphericalJointRTTI.h"

namespace BansheeEngine
{
	CSphericalJoint::CSphericalJoint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("SphericalJoint");
	}

	LimitConeRange CSphericalJoint::getLimit() const
	{
		return mLimit;
	}

	void CSphericalJoint::setLimit(const LimitConeRange& limit)
	{
		if (limit == mLimit)
			return;

		mLimit = limit;

		if (mInternal != nullptr)
			_getInternal()->setLimit(limit);
	}
	
	void CSphericalJoint::setFlag(SphericalJoint::Flag flag, bool enabled)
	{
		bool isEnabled = ((UINT32)mFlag & (UINT32)flag) != 0;
		if (isEnabled == enabled)
			return;

		if (enabled)
			mFlag = (SphericalJoint::Flag)((UINT32)mFlag | (UINT32)flag);
		else
			mFlag = (SphericalJoint::Flag)((UINT32)mFlag & ~(UINT32)flag);

		if (mInternal != nullptr)
			_getInternal()->setFlag(flag, enabled);
	}

	bool CSphericalJoint::hasFlag(SphericalJoint::Flag flag) const
	{
		return ((UINT32)mFlag & (UINT32)flag) != 0;
	}

	SPtr<Joint> CSphericalJoint::createInternal()
	{
		return SphericalJoint::create();
	}

	RTTITypeBase* CSphericalJoint::getRTTIStatic()
	{
		return CSphericalJointRTTI::instance();
	}

	RTTITypeBase* CSphericalJoint::getRTTI() const
	{
		return CSphericalJoint::getRTTIStatic();
	}
}
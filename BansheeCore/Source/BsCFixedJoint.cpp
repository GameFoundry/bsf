//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCFixedJoint.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsCFixedJointRTTI.h"

namespace BansheeEngine
{
	CFixedJoint::CFixedJoint(const HSceneObject& parent)
		: CJoint(parent)
	{
		setName("FixedJoint");
	}

	SPtr<Joint> CFixedJoint::createInternal()
	{
		return FixedJoint::create();
	}

	RTTITypeBase* CFixedJoint::getRTTIStatic()
	{
		return CFixedJointRTTI::instance();
	}

	RTTITypeBase* CFixedJoint::getRTTI() const
	{
		return CFixedJoint::getRTTIStatic();
	}
}
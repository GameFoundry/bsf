//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCFixedJoint.h"
#include "BsSceneObject.h"
#include "BsCRigidbody.h"
#include "BsCFixedJointRTTI.h"

namespace BansheeEngine
{
	CFixedJoint::CFixedJoint() 
		:CJoint(mDesc)
	{ }

	CFixedJoint::CFixedJoint(const HSceneObject& parent)
		: CJoint(parent, mDesc)
	{
		setName("FixedJoint");
	}

	SPtr<Joint> CFixedJoint::createInternal()
	{
		SPtr<Joint> joint = FixedJoint::create(mDesc);

		joint->_setOwner(PhysicsOwnerType::Component, this);
		return joint;
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
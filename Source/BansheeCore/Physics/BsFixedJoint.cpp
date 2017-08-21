//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Physics/BsFixedJoint.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<FixedJoint> FixedJoint::create(const FIXED_JOINT_DESC& desc)
	{
		return gPhysics().createFixedJoint(desc);
	}
}
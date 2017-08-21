//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Physics/BsDistanceJoint.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<DistanceJoint> DistanceJoint::create(const DISTANCE_JOINT_DESC& desc)
	{
		return gPhysics().createDistanceJoint(desc);
	}
}
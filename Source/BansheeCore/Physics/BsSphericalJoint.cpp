//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Physics/BsSphericalJoint.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<SphericalJoint> SphericalJoint::create(const SPHERICAL_JOINT_DESC& desc)
	{
		return gPhysics().createSphericalJoint(desc);
	}
}
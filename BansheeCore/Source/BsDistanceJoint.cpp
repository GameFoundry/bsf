//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsDistanceJoint.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SPtr<DistanceJoint> DistanceJoint::create()
	{
		return gPhysics().createDistanceJoint();
	}
}
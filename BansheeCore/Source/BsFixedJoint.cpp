//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsFixedJoint.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SPtr<FixedJoint> FixedJoint::create()
	{
		return gPhysics().createFixedJoint();
	}
}
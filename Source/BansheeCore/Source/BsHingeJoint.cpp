//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsHingeJoint.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SPtr<HingeJoint> HingeJoint::create()
	{
		return gPhysics().createHingeJoint();
	}
}
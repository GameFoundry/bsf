//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Physics/BsD6Joint.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<D6Joint> D6Joint::create(const D6_JOINT_DESC& desc)
	{
		return gPhysics().createD6Joint(desc);
	}
}
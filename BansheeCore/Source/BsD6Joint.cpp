//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD6Joint.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SPtr<D6Joint> D6Joint::create()
	{
		return gPhysics().createD6Joint();
	}
}
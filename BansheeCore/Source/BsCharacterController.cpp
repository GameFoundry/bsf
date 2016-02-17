//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCharacterController.h"
#include "BsPhysics.h"

namespace BansheeEngine
{
	SPtr<CharacterController> CharacterController::create()
	{
		return gPhysics().createCharacterController();
	}
}
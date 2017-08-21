//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Physics/BsCharacterController.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<CharacterController> CharacterController::create(const CHAR_CONTROLLER_DESC& desc)
	{
		return gPhysics().createCharacterController(desc);
	}
}
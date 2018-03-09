//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsCharacterController.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<CharacterController> CharacterController::create(const CHAR_CONTROLLER_DESC& desc)
	{
		return gPhysics().createCharacterController(desc);
	}
}
//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsSceneManager.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	SceneManager& gBsSceneManager()
	{
		return static_cast<SceneManager&>(gSceneManager());
	}
}

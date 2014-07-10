//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLPrerequisites.h"
#include "BsGLRenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" BS_RSGL_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
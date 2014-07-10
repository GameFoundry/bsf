//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D9Prerequisites.h"
#include "BsD3D9RenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" BS_D3D9_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
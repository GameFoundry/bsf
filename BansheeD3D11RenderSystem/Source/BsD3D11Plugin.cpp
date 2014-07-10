//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsD3D11Prerequisites.h"
#include "BsD3D11RenderSystemFactory.h"

namespace BansheeEngine
{
	extern "C" BS_D3D11_EXPORT const String& getPluginName()
	{
		return SystemName;
	}
}
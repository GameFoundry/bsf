//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsMonoPrerequisites.h"

namespace BansheeEngine
{
	extern "C" BS_MONO_EXPORT const String& getPluginName()
	{
		static String pluginName = "BansheeMono";
		return pluginName;
	}
}
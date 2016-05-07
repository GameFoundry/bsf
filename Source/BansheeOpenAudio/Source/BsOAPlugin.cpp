//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAPrerequisites.h"

namespace BansheeEngine
{
	/**	Returns a name of the plugin. */
	extern "C" BS_OA_EXPORT const char* getPluginName()
	{
		static const char* pluginName = "OpenAudio";
		return pluginName;
	}

	/**	Entry point to the plugin. Called by the engine when the plugin is loaded. */
	extern "C" BS_OA_EXPORT void* loadPlugin()
	{
		// TODO

		return nullptr;
	}
}
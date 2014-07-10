//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsOISPrerequisites.h"
#include "BsInputHandlerOIS.h"
#include "BsRenderWindow.h"
#include "BsInput.h"

namespace BansheeEngine
{
	/**
	 * @brief	Returns a name of the plugin.
	 */
	extern "C" BS_OIS_EXPORT const String& getPluginName()
	{
		static String pluginName = "OISInput";
		return pluginName;
	}

	/**
	 * @brief	Entry point to the plugin. Called by the engine when the plugin is loaded.
	 */
	extern "C" BS_OIS_EXPORT void* loadPlugin(void* primaryWindowPtr)
	{
		RenderWindow* primaryWindow = (RenderWindow*)primaryWindowPtr;

		if (primaryWindow == nullptr)
			assert(false && "Unable to get window handle. No active window exists!");

		UINT64 windowId = 0;
		primaryWindow->getCustomAttribute("WINDOW", &windowId);

		// TODO - Window handles in Windows are 64 bits when compiled as x64, but OIS only accepts a 32bit value. Is this okay?
		std::shared_ptr<RawInputHandler> inputHandler = bs_shared_ptr<InputHandlerOIS>((UINT32)windowId);

		gInput()._registerRawInputHandler(inputHandler);

		return nullptr;
	}
}
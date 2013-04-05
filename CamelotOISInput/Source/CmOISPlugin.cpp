#include "CmOISPrerequisites.h"
#include "CmInputHandlerOIS.h"
#include "CmApplication.h"
#include "CmInput.h"

namespace CamelotEngine
{
	extern "C" CM_OIS_EXPORT const String& getPluginName()
	{
		static String pluginName = "OISInput";
		return pluginName;
	}

	extern "C" CM_OIS_EXPORT void loadPlugin()
	{
		// TODO - Window handles in Windows are 64 bits when compiled as x64, but OIS only accepts a 32bit value. Is this okay?
		UINT32 windowId = (UINT32)gApplication().getAppWindowId();

		InputHandlerOIS* inputHandler = CM_NEW(InputHandlerOIS, GenAlloc) InputHandlerOIS(windowId);
		gInput().registerInputHandler(InputHandlerPtr(inputHandler, &MemAllocDeleter<InputHandlerOIS, GenAlloc>::deleter));
	}
}
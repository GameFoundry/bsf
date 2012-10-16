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
		InputHandlerOIS* inputHandler = new InputHandlerOIS(gApplication().getAppWindowId());
		gInput().registerInputHandler(inputHandler);
	}
}
#include "CmOISPrerequisites.h"
#include "CmInputHandlerOIS.h"
#include "CmApplication.h"
#include "CmInput.h"

namespace BansheeEngine
{
	extern "C" CM_OIS_EXPORT const String& getPluginName()
	{
		static String pluginName = "OISInput";
		return pluginName;
	}

	extern "C" CM_OIS_EXPORT void* loadPlugin()
	{
		// TODO - Window handles in Windows are 64 bits when compiled as x64, but OIS only accepts a 32bit value. Is this okay?
		UINT32 windowId = (UINT32)gApplication().getAppWindowId();

		std::shared_ptr<RawInputHandler> inputHandler = cm_shared_ptr<InputHandlerOIS>(windowId);

		gInput()._registerRawInputHandler(inputHandler);

		return nullptr;
	}
}
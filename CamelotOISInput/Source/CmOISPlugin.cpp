#include "CmOISPrerequisites.h"
#include "CmInputHandlerOIS.h"
#include "CmRenderWindow.h"
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
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();

		if (primaryWindow == nullptr)
			assert(false && "Unable to get window handle. No active window exists!");

		UINT64 windowId = 0;
		primaryWindow->getCustomAttribute("WINDOW", &windowId);

		// TODO - Window handles in Windows are 64 bits when compiled as x64, but OIS only accepts a 32bit value. Is this okay?
		std::shared_ptr<RawInputHandler> inputHandler = cm_shared_ptr<InputHandlerOIS>((UINT32)windowId);

		gInput()._registerRawInputHandler(inputHandler);

		return nullptr;
	}
}
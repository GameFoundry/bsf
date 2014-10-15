#include "BsScriptEditorPrerequisites.h"
#include "BsEditorScriptManager.h"

namespace BansheeEngine
{
	extern "C" BS_SCR_BED_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEditor";
		return pluginName;
	}

	extern "C" BS_SCR_BED_EXPORT void* loadPlugin()
	{
		EditorScriptManager::startUp();

		return nullptr;
	}

	extern "C" BS_SCR_BED_EXPORT void updatePlugin()
	{
		EditorScriptManager::instance().update();
	}

	extern "C" BS_SCR_BED_EXPORT void shutdownPlugin()
	{
		EditorScriptManager::shutDown();
	}

	extern "C" BS_SCR_BED_EXPORT void unloadPlugin()
	{
		
	}
}
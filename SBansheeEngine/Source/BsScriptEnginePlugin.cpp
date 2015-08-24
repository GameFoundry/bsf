#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObjectManager.h"
#include "BsEngineScriptLibrary.h"
#include "BsScriptManager.h"

namespace BansheeEngine
{
	extern "C" BS_SCR_BE_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEngine";
		return pluginName;
	}

	extern "C" BS_SCR_BE_EXPORT void* loadPlugin()
	{
		SPtr<EngineScriptLibrary> library = bs_shared_ptr_new<EngineScriptLibrary>();
		ScriptManager::instance()._setScriptLibrary(library);

		return nullptr;
	}

	extern "C" BS_SCR_BE_EXPORT void updatePlugin()
	{
		ScriptObjectManager::instance().update();
	}
}
#include "BsMonoPrerequisites.h"
#include "BsMonoScriptManager.h"

namespace BansheeEngine
{
	extern "C" BS_MONO_EXPORT const String& getPluginName()
	{
		static String pluginName = "BansheeMono";
		return pluginName;
	}

	extern "C" BS_MONO_EXPORT void* loadPlugin()
	{
		std::shared_ptr<MonoScriptSystem> mono = cm_shared_ptr<MonoScriptSystem>();
		ScriptManager::instance().initialize(mono);

		return nullptr;
	}
}
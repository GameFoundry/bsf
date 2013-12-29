#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	extern "C" BS_SCR_BE_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEngine";
		return pluginName;
	}

	extern "C" BS_SCR_BE_EXPORT void* loadPlugin()
	{
		const CM::String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEngine.dll";
		const CM::String ENGINE_ASSEMBLY_NAME = "MBansheeEngine";
		const CM::String ASSEMBLY_ENTRY_POINT = "Program::Main";
		
		MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME, ASSEMBLY_ENTRY_POINT);

		return nullptr;
	}
}
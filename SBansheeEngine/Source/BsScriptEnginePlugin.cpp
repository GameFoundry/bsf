#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"

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
		const CM::String ENGINE_ASSEMBLY_NAME = BansheeEngineAssemblyName;
		const CM::String ASSEMBLY_ENTRY_POINT = "Program::Main";
		
		MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME, ASSEMBLY_ENTRY_POINT);

		RuntimeScriptObjects::startUp(cm_new<RuntimeScriptObjects>());
		ScriptResourceManager::startUp(cm_new<ScriptResourceManager>());
		ScriptGameObjectManager::startUp(cm_new<ScriptGameObjectManager>());

		RuntimeScriptObjects::instance().refreshScriptObjects(BansheeEngineAssemblyName);

		return nullptr;
	}

	extern "C" BS_SCR_BE_EXPORT void unloadPlugin()
	{
		ScriptGameObjectManager::shutDown();
		ScriptResourceManager::shutDown();
		RuntimeScriptObjects::shutDown();
	}
}
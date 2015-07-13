#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsManagedResourceManager.h"
#include "BsScriptManager.h"
#include "BsScriptInput.h"
#include "BsScriptVirtualInput.h"
#include "BsScriptObjectManager.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	extern "C" BS_SCR_BE_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEngine";
		return pluginName;
	}

	extern "C" BS_SCR_BE_EXPORT void* loadPlugin()
	{
		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		const String ASSEMBLY_ENTRY_POINT = "Program::Start";

		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(engineAssemblyPath.toString(), ENGINE_ASSEMBLY);

		// TODO - Load Game assembly (gApplication().getGameAssemblyPath())

		ScriptObjectManager::startUp();
		ManagedResourceManager::startUp();
		ScriptAssemblyManager::startUp();
		ScriptResourceManager::startUp();
		ScriptGameObjectManager::startUp();
		ScriptInput::startUp();
		ScriptVirtualInput::startUp();

		ScriptAssemblyManager::instance().loadAssemblyInfo(ENGINE_ASSEMBLY);

		bansheeEngineAssembly.invoke(ASSEMBLY_ENTRY_POINT);

		return nullptr;
	}

	extern "C" BS_SCR_BE_EXPORT void updatePlugin()
	{
		ScriptObjectManager::instance().update();
	}

	extern "C" BS_SCR_BE_EXPORT void unloadPlugin()
	{
		ScriptVirtualInput::shutDown();
		ScriptInput::shutDown();
		ManagedResourceManager::shutDown();
		ScriptManager::instance().destroy();
		ScriptGameObjectManager::shutDown();
		ScriptResourceManager::shutDown();
		ScriptAssemblyManager::shutDown();
		ScriptObjectManager::shutDown();
	}
}
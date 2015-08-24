#include "BsEngineScriptLibrary.h"
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
#include "BsGameResourceManager.h"
#include "BsApplication.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	void EngineScriptLibrary::initialize()
	{
		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		const String ASSEMBLY_ENTRY_POINT = "Program::Start";

		MonoManager::startUp();
		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(engineAssemblyPath.toString(), ENGINE_ASSEMBLY);

		GameResourceManager::startUp();
		ScriptObjectManager::startUp();
		ManagedResourceManager::startUp();
		ScriptAssemblyManager::startUp();
		ScriptResourceManager::startUp();
		ScriptGameObjectManager::startUp();
		ScriptInput::startUp();
		ScriptVirtualInput::startUp();

		ScriptAssemblyManager::instance().loadAssemblyInfo(ENGINE_ASSEMBLY);
		bansheeEngineAssembly.invoke(ASSEMBLY_ENTRY_POINT);
	}

	void EngineScriptLibrary::reload()
	{
		Path engineAssemblyPath = gApplication().getEngineAssemblyPath();
		Path gameAssemblyPath = gApplication().getGameAssemblyPath();

		Vector<std::pair<String, Path>> assemblies;
		assemblies.push_back({ ENGINE_ASSEMBLY, engineAssemblyPath });

		if (FileSystem::exists(gameAssemblyPath))
			assemblies.push_back({ SCRIPT_GAME_ASSEMBLY, gameAssemblyPath });

		ScriptObjectManager::instance().refreshAssemblies(assemblies);
	}

	void EngineScriptLibrary::destroy()
	{
		ScriptVirtualInput::shutDown();
		ScriptInput::shutDown();
		ManagedResourceManager::shutDown();
		MonoManager::shutDown();
		ScriptObjectManager::instance().processFinalizedObjects();

		ScriptGameObjectManager::shutDown();
		ScriptResourceManager::shutDown();
		ScriptAssemblyManager::shutDown();
		ScriptObjectManager::shutDown();
		GameResourceManager::shutDown();
	}
}
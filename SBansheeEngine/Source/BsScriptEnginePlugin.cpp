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

// DEBUG ONLY
#include "BsScriptSceneObject.h"
#include "BsSceneObject.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	void unitTest1_GameObjectClone(MonoObject* instance)
	{
		ScriptSceneObject* nativeInstance = ScriptSceneObject::toNative(instance);

		HSceneObject SO = static_object_cast<SceneObject>(nativeInstance->getNativeHandle());
		HSceneObject cloneSO = SO->clone();

		cloneSO->setParent(SO);
	}

	extern "C" BS_SCR_BE_EXPORT const String& getPluginName()
	{
		static String pluginName = "SBansheeEngine";
		return pluginName;
	}

	extern "C" BS_SCR_BE_EXPORT void* loadPlugin()
	{
		const String ENGINE_ASSEMBLY_PATH = "..\\..\\Assemblies\\MBansheeEngine.dll";
		const String ENGINE_ASSEMBLY_NAME = BansheeEngineAssemblyName;
		const String ASSEMBLY_ENTRY_POINT = "Program::Start";

		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);

		// DEBUG ONLY
		mono_add_internal_call("BansheeEngine.Program::UnitTest1_GameObjectClone", &unitTest1_GameObjectClone);

		ScriptObjectManager::startUp();
		ManagedResourceManager::startUp();
		ScriptAssemblyManager::startUp();
		ScriptResourceManager::startUp();
		ScriptGameObjectManager::startUp();
		ScriptInput::startUp();
		ScriptVirtualInput::startUp();

		ScriptAssemblyManager::instance().loadAssemblyInfo(BansheeEngineAssemblyName);

		bansheeEngineAssembly.invoke(ASSEMBLY_ENTRY_POINT);

		return nullptr;
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
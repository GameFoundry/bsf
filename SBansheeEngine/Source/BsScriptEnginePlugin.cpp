#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"

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
		const String ASSEMBLY_ENTRY_POINT = "Program::Main";

		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);

		// DEBUG ONLY
		mono_add_internal_call("BansheeEngine.Program::UnitTest1_GameObjectClone", &unitTest1_GameObjectClone);

		RuntimeScriptObjects::startUp();
		ScriptResourceManager::startUp();
		ScriptGameObjectManager::startUp();

		RuntimeScriptObjects::instance().refreshScriptObjects(BansheeEngineAssemblyName);

		bansheeEngineAssembly.invoke(ASSEMBLY_ENTRY_POINT);

		return nullptr;
	}

	extern "C" BS_SCR_BE_EXPORT void unloadPlugin()
	{
		ScriptGameObjectManager::shutDown();
		ScriptResourceManager::shutDown();
		RuntimeScriptObjects::shutDown();
	}
}
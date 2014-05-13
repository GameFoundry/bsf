#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"

// DEBUG ONLY
#include "BsScriptSceneObject.h"
#include "CmSceneObject.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	void dbgTestComponentClone(MonoObject* instance)
	{
		ScriptSceneObject* nativeInstance = ScriptSceneObject::toNative(instance);

		HSceneObject SO = static_object_cast<SceneObject>(nativeInstance->getNativeHandle());
		HSceneObject cloneSO = SO->clone();

		cloneSO->setParent(SO);
	}

	void reportDbgValue(int a, MonoString* b, int a2, MonoString* b2)
	{
		WString bStr = MonoUtil::monoToWString(b);
		WString b2Str = MonoUtil::monoToWString(b2);

		int end = 5;
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
		mono_add_internal_call("BansheeEngine.Program::dbgTestComponentClone", &dbgTestComponentClone);
		mono_add_internal_call("BansheeEngine.Program::reportDbgValue", &reportDbgValue);

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
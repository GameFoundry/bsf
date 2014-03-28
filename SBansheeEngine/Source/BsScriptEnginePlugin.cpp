#include "BsScriptEnginePrerequisites.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsRuntimeScriptObjects.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"

// DEBUG ONLY
#include "BsScriptSceneObject.h"
#include "CmSceneObject.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	void dbgTestComponentClone(MonoObject* instance)
	{
		ScriptSceneObject* nativeInstance = ScriptSceneObject::toNative(instance);

		CM::HSceneObject SO = static_object_cast<CM::SceneObject>(nativeInstance->getNativeHandle());
		CM::HSceneObject cloneSO = SO->clone();

		cloneSO->setParent(SO);
	}

	void reportDbgValue(int dbgValue, int dbgValue2, int dbgValue3, MonoReflectionType* type)
	{
		::MonoClass* monoClass = mono_type_get_class(mono_reflection_type_get_type(type));

		String className = mono_class_get_name(monoClass);
		
		int a = dbgValue;
	}

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

		MonoAssembly& bansheeEngineAssembly = MonoManager::instance().loadAssembly(ENGINE_ASSEMBLY_PATH, ENGINE_ASSEMBLY_NAME);

		// DEBUG ONLY
		mono_add_internal_call("BansheeEngine.Program::dbgTestComponentClone", &dbgTestComponentClone);
		mono_add_internal_call("BansheeEngine.Program::reportDbgValue", &reportDbgValue);

		RuntimeScriptObjects::startUp(cm_new<RuntimeScriptObjects>());
		ScriptResourceManager::startUp(cm_new<ScriptResourceManager>());
		ScriptGameObjectManager::startUp(cm_new<ScriptGameObjectManager>());

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
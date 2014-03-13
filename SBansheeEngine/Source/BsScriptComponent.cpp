#include "BsScriptComponent.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsManagedComponent.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptComponent::ScriptComponent(const CM::GameObjectHandle<ManagedComponent>& managedComponent)
		:mManagedComponent(managedComponent)
	{

	}

	void ScriptComponent::initMetaData()
	{
		metaData = ScriptMeta(BansheeEngineAssemblyName, "BansheeEngine", "Component", &ScriptComponent::initRuntimeData);

		MonoManager::registerScriptType(&metaData);
	}

	void ScriptComponent::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptComponent::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptComponent::internal_destroyInstance);
	}

	void ScriptComponent::internal_createInstance(MonoObject* instance, MonoString* ns, MonoString* typeName)
	{
		// TODO - Just a placeholder

		ScriptComponent* nativeInstance = new (cm_alloc<ScriptComponent>()) ScriptComponent(GameObjectHandle<ManagedComponent>());
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptComponent::internal_destroyInstance(ScriptComponent* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}

	void ScriptComponent::setNativeHandle(const CM::HGameObject& gameObject)
	{
		mManagedComponent = static_object_cast<ManagedComponent>(gameObject);
	}
}
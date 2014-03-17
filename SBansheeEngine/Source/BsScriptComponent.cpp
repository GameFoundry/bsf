#include "BsScriptComponent.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptSceneObject.h"
#include "BsManagedComponent.h"
#include "CmSceneObject.h"

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
		metaData.scriptClass->addInternalCall("Internal_AddComponent", &ScriptComponent::internal_addComponent);
		metaData.scriptClass->addInternalCall("Internal_RemoveComponent", &ScriptComponent::internal_destroyInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptComponent::internal_destroyInstance);
	}

	MonoObject* ScriptComponent::internal_addComponent(MonoObject* parentSceneObject, MonoString* ns, MonoString* typeName)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		String strNs = toString(MonoUtil::monoToWString(ns));
		String strTypeName = toString(MonoUtil::monoToWString(typeName));
		String fullTypeName = strNs + "." + strTypeName;

		// We only allow single component per type
		const Vector<HComponent>::type& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				if(managedComponent->getManagedFullTypeName() == fullTypeName)
				{
					LOGWRN("Attempting to add a component \"" + fullTypeName + "\" that already exists on SceneObject \"" + so->getName() + "\"");
					return managedComponent->getManagedInstance();
				}
			}
		}

		GameObjectHandle<ManagedComponent> mc = so->addComponent<ManagedComponent>(strNs, strTypeName);
		ScriptComponent* nativeInstance = ScriptGameObjectManager::instance().createScriptComponent(mc);
		
		return nativeInstance->getManagedInstance();
	}

	void ScriptComponent::internal_removeComponent(MonoObject* parentSceneObject, MonoString* ns, MonoString* typeName)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		String strNs = toString(MonoUtil::monoToWString(ns));
		String strTypeName = toString(MonoUtil::monoToWString(typeName));
		String fullTypeName = strNs + "." + strTypeName;

		// We only allow single component per type
		const Vector<HComponent>::type& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				if(managedComponent->getManagedFullTypeName() == fullTypeName)
				{
					managedComponent->destroy();
					return;
				}
			}
		}

		LOGWRN("Attempting to remove a component \"" + fullTypeName + "\" that doesn't exists on SceneObject \"" + so->getName() + "\"");
	}

	void ScriptComponent::internal_destroyInstance(ScriptComponent* nativeInstance)
	{
		ScriptGameObjectManager::instance().destroyScriptGameObject(nativeInstance);
	}

	void ScriptComponent::setNativeHandle(const CM::HGameObject& gameObject)
	{
		mManagedComponent = static_object_cast<ManagedComponent>(gameObject);
	}
}
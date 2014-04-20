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

namespace BansheeEngine
{
	ScriptComponent::ScriptComponent(const GameObjectHandle<ManagedComponent>& managedComponent)
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
		metaData.scriptClass->addInternalCall("Internal_GetComponent", &ScriptComponent::internal_getComponent);
		metaData.scriptClass->addInternalCall("Internal_GetComponents", &ScriptComponent::internal_getComponents);
		metaData.scriptClass->addInternalCall("Internal_RemoveComponent", &ScriptComponent::internal_removeComponent);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptComponent::internal_destroyInstance);
	}

	MonoObject* ScriptComponent::internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		// We only allow single component per type
		const Vector<HComponent>::type& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				if(managedComponent->getRuntimeType() == type)
				{
					LOGWRN("Attempting to add a component that already exists on SceneObject \"" + so->getName() + "\"");
					return managedComponent->getManagedInstance();
				}
			}
		}

		GameObjectHandle<ManagedComponent> mc = so->addComponent<ManagedComponent>(type);
		ScriptComponent* nativeInstance = ScriptGameObjectManager::instance().createScriptComponent(mc);
		
		return nativeInstance->getManagedInstance();
	}

	MonoObject* ScriptComponent::internal_getComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		const Vector<HComponent>::type& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				if(managedComponent->getRuntimeType() == type)
				{
					return managedComponent->getManagedInstance();
				}
			}
		}

		return nullptr;
	}

	MonoArray* ScriptComponent::internal_getComponents(MonoObject* parentSceneObject)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		const Vector<HComponent>::type& mComponents = so->getComponents();
		Vector<MonoObject*>::type managedComponents;

		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				managedComponents.push_back(managedComponent->getManagedInstance());
			}
		}

		MonoArray* componentArray = mono_array_new(MonoManager::instance().getDomain(), 
			metaData.scriptClass->_getInternalClass(), (UINT32)managedComponents.size());

		for(UINT32 i = 0; i < (UINT32)managedComponents.size(); i++)
		{
			void* elemAddr = mono_array_addr_with_size(componentArray, sizeof(MonoObject*), i);
			memcpy(elemAddr, &managedComponents[i], sizeof(MonoObject*));
		}

		return componentArray;
	}

	void ScriptComponent::internal_removeComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		// We only allow single component per type
		const Vector<HComponent>::type& mComponents = so->getComponents();
		for(auto& component : mComponents)
		{
			if(component->getTypeId() == TID_ManagedComponent)
			{
				GameObjectHandle<ManagedComponent> managedComponent = static_object_cast<ManagedComponent>(component);

				if(managedComponent->getRuntimeType() == type)
				{
					managedComponent->destroy();
					return;
				}
			}
		}

		LOGWRN("Attempting to remove a component that doesn't exists on SceneObject \"" + so->getName() + "\"");
	}

	void ScriptComponent::internal_destroyInstance(ScriptComponent* nativeInstance)
	{
		ScriptGameObjectManager::instance().destroyScriptGameObject(nativeInstance);
	}

	void ScriptComponent::setNativeHandle(const HGameObject& gameObject)
	{
		mManagedComponent = static_object_cast<ManagedComponent>(gameObject);
	}
}
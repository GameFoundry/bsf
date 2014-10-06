#include "BsScriptComponent.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsScriptSceneObject.h"
#include "BsManagedComponent.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	ScriptComponent::ScriptComponent(MonoObject* instance, const GameObjectHandle<ManagedComponent>& managedComponent)
		:ScriptObject(instance), mManagedComponent(managedComponent)
	{ }

	void ScriptComponent::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_AddComponent", &ScriptComponent::internal_addComponent);
		metaData.scriptClass->addInternalCall("Internal_GetComponent", &ScriptComponent::internal_getComponent);
		metaData.scriptClass->addInternalCall("Internal_GetComponents", &ScriptComponent::internal_getComponents);
		metaData.scriptClass->addInternalCall("Internal_RemoveComponent", &ScriptComponent::internal_removeComponent);
		metaData.scriptClass->addInternalCall("Internal_GetSceneObject", &ScriptComponent::internal_getSceneObject);
	}

	MonoObject* ScriptComponent::internal_addComponent(MonoObject* parentSceneObject, MonoReflectionType* type)
	{
		ScriptSceneObject* scriptSO = ScriptSceneObject::toNative(parentSceneObject);
		HSceneObject so = static_object_cast<SceneObject>(scriptSO->getNativeHandle());

		// We only allow single component per type
		const Vector<HComponent>& mComponents = so->getComponents();
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

		const Vector<HComponent>& mComponents = so->getComponents();
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

		const Vector<HComponent>& mComponents = so->getComponents();
		Vector<MonoObject*> managedComponents;

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
		const Vector<HComponent>& mComponents = so->getComponents();
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

	MonoObject* ScriptComponent::internal_getSceneObject(ScriptComponent* nativeInstance)
	{
		HSceneObject sceneObject = nativeInstance->mManagedComponent->sceneObject();

		ScriptSceneObject* scriptSO = ScriptGameObjectManager::instance().getScriptSceneObject(sceneObject);
		if (scriptSO == nullptr)
			scriptSO = ScriptGameObjectManager::instance().createScriptSceneObject(sceneObject);

		return scriptSO->getManagedInstance();
	}

	void ScriptComponent::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		ScriptGameObjectManager::instance().destroyScriptGameObject(this);
	}

	void ScriptComponent::setNativeHandle(const HGameObject& gameObject)
	{
		mManagedComponent = static_object_cast<ManagedComponent>(gameObject);
	}
}
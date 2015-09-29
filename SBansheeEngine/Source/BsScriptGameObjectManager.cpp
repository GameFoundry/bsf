#include "BsScriptGameObjectManager.h"
#include "BsScriptGameObject.h"
#include "BsScriptComponent.h"
#include "BsScriptSceneObject.h"
#include "BsGameObject.h"
#include "BsComponent.h"
#include "BsManagedComponent.h"
#include "BsSceneObject.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"
#include "BsScriptAssemblyManager.h"
#include "BsScriptObjectManager.h"

namespace BansheeEngine
{
	ScriptGameObjectManager::ScriptGameObjectEntry::ScriptGameObjectEntry()
		:instance(nullptr), isComponent(false)
	{ }

	ScriptGameObjectManager::ScriptGameObjectEntry::ScriptGameObjectEntry(ScriptGameObjectBase* instance, bool isComponent)
		:instance(instance), isComponent(isComponent)
	{ }

	ScriptGameObjectManager::ScriptGameObjectManager()
	{
		// Calls OnReset on all components after assembly reload happens
		mOnAssemblyReloadDoneConn = ScriptObjectManager::instance().onRefreshComplete.connect(
			std::bind(&ScriptGameObjectManager::sendComponentResetEvents, this));
	}

	ScriptGameObjectManager::~ScriptGameObjectManager()
	{
		mOnAssemblyReloadDoneConn.disconnect();
	}

	ScriptSceneObject* ScriptGameObjectManager::getOrCreateScriptSceneObject(const HSceneObject& sceneObject)
	{
		ScriptSceneObject* so = getScriptSceneObject(sceneObject);
		if (so != nullptr)
			return so;

		return createScriptSceneObject(sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(const HSceneObject& sceneObject)
	{
		MonoClass* sceneObjectClass = ScriptAssemblyManager::instance().getSceneObjectClass();
		MonoObject* instance = sceneObjectClass->createInstance();

		return createScriptSceneObject(instance, sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject)
	{
		ScriptGameObjectBase* go = getScriptGameObject(sceneObject.getInstanceId());
		if (go != nullptr)
		{
			BS_EXCEPT(InvalidStateException, "Script component for this SceneObject already exists.");
		}

		ScriptSceneObject* nativeInstance = new (bs_alloc<ScriptSceneObject>()) ScriptSceneObject(existingInstance, sceneObject);
		mScriptGameObjects[sceneObject.getInstanceId()] = ScriptGameObjectEntry(nativeInstance, false);

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::createScriptComponent(MonoObject* existingInstance, const GameObjectHandle<ManagedComponent>& component)
	{
		ScriptGameObjectBase* go = getScriptGameObject(component.getInstanceId());
		if (go != nullptr)
		{
			BS_EXCEPT(InvalidStateException, "Script component for this Component already exists.");
		}

		ScriptComponent* nativeInstance = new (bs_alloc<ScriptComponent>()) ScriptComponent(existingInstance);
		nativeInstance->setNativeHandle(component);
		mScriptGameObjects[component->getInstanceId()] = ScriptGameObjectEntry(nativeInstance, true);

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const
	{
		ScriptGameObjectBase* go = getScriptGameObject(component.getInstanceId());
		return static_cast<ScriptComponent*>(go);
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(UINT64 instanceId) const
	{
		ScriptGameObjectBase* go = getScriptGameObject(instanceId);
		return static_cast<ScriptComponent*>(go);
	}

	ScriptSceneObject* ScriptGameObjectManager::getScriptSceneObject(const HSceneObject& sceneObject) const
	{
		ScriptGameObjectBase* go = getScriptGameObject(sceneObject.getInstanceId());
		return static_cast<ScriptSceneObject*>(go);
	}

	ScriptGameObjectBase* ScriptGameObjectManager::getScriptGameObject(UINT64 instanceId) const
	{
		auto findIter = mScriptGameObjects.find(instanceId);
		if (findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second.instance);

		return nullptr;
	}

	void ScriptGameObjectManager::destroyScriptGameObject(ScriptGameObjectBase* gameObject)
	{
		UINT64 instanceId = gameObject->getNativeHandle().getInstanceId();
		mScriptGameObjects.erase(instanceId);

		bs_delete(gameObject);
	}

	void ScriptGameObjectManager::sendComponentResetEvents()
	{
		for (auto& scriptObjectEntry : mScriptGameObjects)
		{
			const ScriptGameObjectEntry& entry = scriptObjectEntry.second;

			if (!entry.isComponent)
				continue;

			ScriptComponent* scriptComponent = static_cast<ScriptComponent*>(entry.instance);
			HManagedComponent component = scriptComponent->getNativeHandle();

			component->triggerOnReset();
		}
	}
}
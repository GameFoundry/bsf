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

	void ScriptGameObjectManager::registerScriptComponent(ScriptComponent* nativeInstance, const GameObjectHandle<ManagedComponent>& component)
	{
		auto findIter = mScriptGameObjects.find(component->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script component for this Component already exists.");

		nativeInstance->setManagedComponent(component);
		mScriptGameObjects[component->getInstanceId()] = ScriptGameObjectEntry(nativeInstance, true);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(const HSceneObject& sceneObject)
	{
		MonoClass* sceneObjectClass = ScriptAssemblyManager::instance().getSceneObjectClass();
		MonoObject* instance = sceneObjectClass->createInstance();

		return createScriptSceneObject(instance, sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject)
	{
		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script SceneObject for this SceneObject already exists.");

		ScriptSceneObject* nativeInstance = new (bs_alloc<ScriptSceneObject>()) ScriptSceneObject(existingInstance, sceneObject);
		mScriptGameObjects[sceneObject.getInstanceId()] = ScriptGameObjectEntry(nativeInstance, false);

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const
	{
		auto findIter = mScriptGameObjects.find(component.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptComponent*>(findIter->second.instance);

		return nullptr;
	}

	ScriptSceneObject* ScriptGameObjectManager::getScriptSceneObject(const HSceneObject& sceneObject) const
	{
		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second.instance);

		return nullptr;
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
		UINT64 idx = gameObject->getNativeHandle().getInstanceId();
		mScriptGameObjects.erase(idx);

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
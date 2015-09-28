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
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if (findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second.instance);

		MonoClass* sceneObjectClass = ScriptAssemblyManager::instance().getSceneObjectClass();
		MonoObject* instance = sceneObjectClass->createInstance();

		ScriptSceneObject* nativeInstance = new (bs_alloc<ScriptSceneObject>()) ScriptSceneObject(instance, sceneObject);
		mScriptGameObjects[sceneObject.getInstanceId()] = ScriptGameObjectEntry(nativeInstance, false);

		return nativeInstance;
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(const HSceneObject& sceneObject)
	{
		MonoClass* sceneObjectClass = ScriptAssemblyManager::instance().getSceneObjectClass();
		MonoObject* instance = sceneObjectClass->createInstance();

		return createScriptSceneObject(instance, sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject)
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script SceneObject for this SceneObject already exists.");

		ScriptSceneObject* nativeInstance = new (bs_alloc<ScriptSceneObject>()) ScriptSceneObject(existingInstance, sceneObject);
		mScriptGameObjects[sceneObject.getInstanceId()] = ScriptGameObjectEntry(nativeInstance, false);

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::createScriptComponent(MonoObject* existingInstance, const GameObjectHandle<ManagedComponent>& component)
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(component->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script component for this Component already exists.");

		ScriptComponent* nativeInstance = new (bs_alloc<ScriptComponent>()) ScriptComponent(existingInstance);
		nativeInstance->setNativeHandle(component);
		mScriptGameObjects[component->getInstanceId()] = ScriptGameObjectEntry(nativeInstance, true);

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(component.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptComponent*>(findIter->second.instance);

		return nullptr;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(UINT64 instanceId) const
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(instanceId);
		if (findIter != mScriptGameObjects.end())
			return static_cast<ScriptComponent*>(findIter->second.instance);

		return nullptr;
	}

	ScriptSceneObject* ScriptGameObjectManager::getScriptSceneObject(const HSceneObject& sceneObject) const
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second.instance);

		return nullptr;
	}

	ScriptGameObjectBase* ScriptGameObjectManager::getScriptGameObject(UINT64 instanceId) const
	{
		Lock<> lock(mMutex);

		auto findIter = mScriptGameObjects.find(instanceId);
		if (findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second.instance);

		return nullptr;
	}

	void ScriptGameObjectManager::destroyScriptGameObject(ScriptGameObjectBase* gameObject)
	{
		Lock<> lock(mMutex);

		UINT64 idx = gameObject->getNativeHandle().getInstanceId();
		mScriptGameObjects.erase(idx);
	}

	void ScriptGameObjectManager::sendComponentResetEvents()
	{
		UnorderedMap<UINT64, ScriptGameObjectEntry> scriptGameObjects;

		{
			Lock<> lock(mMutex);
			scriptGameObjects = mScriptGameObjects;
		}

		for (auto& scriptObjectEntry : scriptGameObjects)
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
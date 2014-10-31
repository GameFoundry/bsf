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

namespace BansheeEngine
{
	ScriptGameObjectManager::ScriptGameObjectManager()
		:mSceneObjectClass(nullptr)
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEngineAssemblyName);
		if(assembly == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot find \"" + String(BansheeEngineAssemblyName) + "\" assembly.");

		mSceneObjectClass = assembly->getClass("BansheeEngine", "SceneObject");

		if(mSceneObjectClass == nullptr)
			BS_EXCEPT(InternalErrorException, "Cannot find managed SceneObject class.");
	}

	ScriptComponent* ScriptGameObjectManager::createScriptComponent(const GameObjectHandle<ManagedComponent>& component)
	{
		auto findIter = mScriptGameObjects.find(component->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script component for this Component already exists.");

		ScriptComponent* nativeInstance = new (bs_alloc<ScriptComponent>()) ScriptComponent(component->getManagedInstance(), component);
		mScriptGameObjects[component->getInstanceId()] = nativeInstance;

		return nativeInstance;
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(const HSceneObject& sceneObject)
	{
		MonoObject* instance = mSceneObjectClass->createInstance();

		return createScriptSceneObject(instance, sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(MonoObject* existingInstance, const HSceneObject& sceneObject)
	{
		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			BS_EXCEPT(InvalidStateException, "Script SceneObject for this SceneObject already exists.");

		ScriptSceneObject* nativeInstance = new (bs_alloc<ScriptSceneObject>()) ScriptSceneObject(existingInstance, sceneObject);
		mScriptGameObjects[sceneObject.getInstanceId()] = nativeInstance;

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(const GameObjectHandle<ManagedComponent>& component) const
	{
		auto findIter = mScriptGameObjects.find(component.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptComponent*>(findIter->second);

		return nullptr;
	}

	ScriptSceneObject* ScriptGameObjectManager::getScriptSceneObject(const HSceneObject& sceneObject) const
	{
		auto findIter = mScriptGameObjects.find(sceneObject.getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second);

		return nullptr;
	}

	ScriptGameObjectBase* ScriptGameObjectManager::getScriptGameObject(UINT64 instanceId) const
	{
		auto findIter = mScriptGameObjects.find(instanceId);
		if (findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second);

		return nullptr;
	}

	void ScriptGameObjectManager::destroyScriptGameObject(ScriptGameObjectBase* gameObject)
	{
		UINT64 idx = gameObject->getNativeHandle().getInstanceId();
		mScriptGameObjects.erase(idx);

		bs_delete(gameObject);
	}
}
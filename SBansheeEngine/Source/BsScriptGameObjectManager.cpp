#include "BsScriptGameObjectManager.h"
#include "BsScriptGameObject.h"
#include "BsScriptComponent.h"
#include "BsScriptSceneObject.h"
#include "CmGameObject.h"
#include "CmComponent.h"
#include "BsManagedComponent.h"
#include "CmSceneObject.h"
#include "BsMonoManager.h"
#include "BsMonoAssembly.h"
#include "BsMonoClass.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptGameObjectManager::ScriptGameObjectManager()
		:mSceneObjectClass(nullptr)
	{
		MonoAssembly* assembly = MonoManager::instance().getAssembly(BansheeEngineAssemblyName);
		if(assembly == nullptr)
			CM_EXCEPT(InternalErrorException, "Cannot find \"" + String(BansheeEngineAssemblyName) + "\" assembly.");

		mSceneObjectClass = assembly->getClass("BansheeEngine", "SceneObject");

		if(mSceneObjectClass == nullptr)
			CM_EXCEPT(InternalErrorException, "Cannot find managed SceneObject class.");
	}

	ScriptComponent* ScriptGameObjectManager::createScriptComponent(const GameObjectHandle<ManagedComponent>& component)
	{
		auto findIter = mScriptGameObjects.find(component->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			CM_EXCEPT(InvalidStateException, "Script component for this Component already exists.");

		ScriptComponent* nativeInstance = new (cm_alloc<ScriptComponent>()) ScriptComponent(component);

		nativeInstance->createInstance(component->getManagedInstance());
		nativeInstance->metaData.thisPtrField->setValue(component->getManagedInstance(), &nativeInstance);

		mScriptGameObjects[component->getInstanceId()] = nativeInstance;

		return nativeInstance;
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(const CM::HSceneObject& sceneObject)
	{
		MonoObject* instance = mSceneObjectClass->createInstance();

		return createScriptSceneObject(instance, sceneObject);
	}

	ScriptSceneObject* ScriptGameObjectManager::createScriptSceneObject(MonoObject* existingInstance, const CM::HSceneObject& sceneObject)
	{
		auto findIter = mScriptGameObjects.find(sceneObject->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			CM_EXCEPT(InvalidStateException, "Script SceneObject for this SceneObject already exists.");

		ScriptSceneObject* nativeInstance = new (cm_alloc<ScriptSceneObject>()) ScriptSceneObject(sceneObject);

		nativeInstance->createInstance(existingInstance);
		nativeInstance->metaData.thisPtrField->setValue(existingInstance, &nativeInstance);

		mScriptGameObjects[sceneObject->getInstanceId()] = nativeInstance;

		return nativeInstance;
	}

	ScriptComponent* ScriptGameObjectManager::getScriptComponent(const GameObjectHandle<ManagedComponent>& component)
	{
		auto findIter = mScriptGameObjects.find(component->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptComponent*>(findIter->second);

		return nullptr;
	}

	ScriptSceneObject* ScriptGameObjectManager::getScriptSceneObject(const CM::HSceneObject& sceneObject)
	{
		auto findIter = mScriptGameObjects.find(sceneObject->getInstanceId());
		if(findIter != mScriptGameObjects.end())
			return static_cast<ScriptSceneObject*>(findIter->second);

		return nullptr;
	}

	void ScriptGameObjectManager::destroyScriptGameObject(ScriptGameObject* gameObject)
	{
		UINT64 idx = gameObject->getNativeHandle()->getInstanceId();
		mScriptGameObjects.erase(idx);

		cm_delete(gameObject);
	}
}
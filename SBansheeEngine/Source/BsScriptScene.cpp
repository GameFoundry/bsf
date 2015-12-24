#include "BsScriptScene.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"
#include "BsMonoUtil.h"
#include "BsSceneManager.h"
#include "BsResources.h"
#include "BsPrefab.h"
#include "BsApplication.h"
#include "BsSceneObject.h"
#include "BsScriptGameObjectManager.h"
#include "BsGameResourceManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptPrefab.h"
#include "BsScriptSceneObject.h"
#include "BsScriptObjectManager.h"

namespace BansheeEngine
{
	const char* ScriptScene::ActiveSceneNameFieldName = "activeSceneName";
	const char* ScriptScene::ActiveSceneUUIDFieldName = "activeSceneUUID";

	HEvent ScriptScene::OnRefreshDomainLoadedConn;
	HEvent ScriptScene::OnRefreshStartedConn;

	String ScriptScene::ActiveSceneUUID;
	WString ScriptScene::ActiveSceneName;

	ScriptScene::ScriptScene(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptScene::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_LoadScene", &ScriptScene::internal_LoadScene);
		metaData.scriptClass->addInternalCall("Internal_GetRoot", &ScriptScene::internal_GetRoot);
		metaData.scriptClass->addInternalCall("Internal_ClearScene", &ScriptScene::internal_ClearScene);
		metaData.scriptClass->addInternalCall("Internal_GetMainCameraSO", &ScriptScene::internal_GetMainCameraSO);
	}

	void ScriptScene::startUp()
	{
		OnRefreshStartedConn = ScriptObjectManager::instance().onRefreshStarted.connect(&onRefreshStarted);
		OnRefreshDomainLoadedConn = ScriptObjectManager::instance().onRefreshDomainLoaded.connect(&onRefreshDomainLoaded);
	}

	void ScriptScene::shutDown()
	{
		OnRefreshStartedConn.disconnect();
		OnRefreshDomainLoadedConn.disconnect();
	}

	MonoObject* ScriptScene::internal_LoadScene(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		HPrefab prefab = GameResourceManager::instance().load<Prefab>(nativePath, true);
		if (prefab.isLoaded(false))
		{
			HSceneObject root = prefab->instantiate();
			gSceneManager()._setRootNode(root);
		}

		if (prefab != nullptr)
		{
			ScriptPrefab* scriptPrefab;
			ScriptResourceManager::instance().getScriptResource(prefab, &scriptPrefab, true);

			return scriptPrefab->getManagedInstance();
		}
		else
		{
			LOGERR("Failed loading scene at path: \"" + nativePath.toString() + "\"");
			return nullptr;
		}
	}

	void ScriptScene::onRefreshStarted()
	{
		MonoField* uuidField = metaData.scriptClass->getField(ActiveSceneUUIDFieldName);
		if (uuidField != nullptr)
			ActiveSceneUUID = MonoUtil::monoToString((MonoString*)uuidField->getValueBoxed(nullptr));

		MonoField* nameField = metaData.scriptClass->getField(ActiveSceneNameFieldName);
		if (nameField != nullptr)
			ActiveSceneName = MonoUtil::monoToWString((MonoString*)nameField->getValueBoxed(nullptr));
	}

	void ScriptScene::onRefreshDomainLoaded()
	{
		MonoField* uuidField = metaData.scriptClass->getField(ActiveSceneUUIDFieldName);
		if (uuidField != nullptr)
			uuidField->setValue(nullptr, MonoUtil::stringToMono(ActiveSceneUUID));

		MonoField* nameField = metaData.scriptClass->getField(ActiveSceneNameFieldName);
		if (nameField != nullptr)
			nameField->setValue(nullptr, MonoUtil::wstringToMono(ActiveSceneName));
	}

	MonoObject* ScriptScene::internal_GetRoot()
	{
		HSceneObject root = SceneManager::instance().getRootNode();

		ScriptSceneObject* scriptRoot = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(root);
		return scriptRoot->getManagedInstance();
	}

	void ScriptScene::internal_ClearScene()
	{
		gSceneManager().clearScene();
	}

	MonoObject* ScriptScene::internal_GetMainCameraSO()
	{
		SceneCameraData cameraData = gSceneManager().getMainCamera();
		if (cameraData.sceneObject == nullptr)
			return nullptr;

		ScriptSceneObject* cameraSo = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(cameraData.sceneObject);
		return cameraSo->getManagedInstance();
	}
}
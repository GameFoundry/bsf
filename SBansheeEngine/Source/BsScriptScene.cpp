#include "BsScriptScene.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
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

namespace BansheeEngine
{
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

	MonoObject* ScriptScene::internal_LoadScene(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		HPrefab prefab = GameResourceManager::instance().load<Prefab>(nativePath);
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
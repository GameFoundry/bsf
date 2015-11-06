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
	}

	MonoObject* ScriptScene::internal_LoadScene(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		HPrefab prefab = GameResourceManager::instance().load<Prefab>(nativePath);
		if (prefab.isLoaded(false))
		{
			HSceneObject root = prefab->instantiate();
			HSceneObject oldRoot = gSceneManager().getRootNode();

			if (root != nullptr)
			{
				UINT32 numChildren = oldRoot->getNumChildren();
				// Make sure to keep persistent objects
				
				UINT32 curIdx = 0;
				for (UINT32 i = 0; i < numChildren; i++)
				{
					HSceneObject child = oldRoot->getChild(curIdx);

					if (child->hasFlag(SOF_Persistent))
						child->setParent(root, false);
				}

				gSceneManager()._setRootNode(root);
				oldRoot->destroy();
			}
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
}
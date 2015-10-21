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
#include "BsGameResourceManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptPrefab.h"

namespace BansheeEngine
{
	ScriptScene::ScriptScene(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptScene::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_LoadScene", &ScriptScene::internal_LoadScene);
		metaData.scriptClass->addInternalCall("Internal_ClearScene", &ScriptScene::internal_ClearScene);
	}

	MonoObject* ScriptScene::internal_LoadScene(MonoString* path)
	{
		Path nativePath = MonoUtil::monoToWString(path);

		HPrefab prefab = GameResourceManager::instance().load<Prefab>(nativePath);
		if (prefab.isLoaded(false))
		{
			HSceneObject root = prefab->instantiate();

			UINT32 numChildren = root->getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
			{
				HSceneObject child = root->getChild(0);
				child->setParent(gSceneManager().getRootNode());
			}

			root->destroy();
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

	void ScriptScene::internal_ClearScene()
	{
		gSceneManager().clearScene();
	}
}
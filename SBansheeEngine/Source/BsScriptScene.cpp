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

	MonoString* ScriptScene::internal_LoadScene(MonoString* path)
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

		MonoString* uuid = MonoUtil::stringToMono(MonoManager::instance().getDomain(), prefab.getUUID());

		// TODO - Return actual prefab
		return uuid;
	}

	void ScriptScene::internal_ClearScene()
	{
		gSceneManager().clearScene();
	}
}
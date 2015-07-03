#include "BsScriptScene.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsSceneManager.h"
#include "BsResources.h"
#include "BsPrefab.h"
#include "BsSceneObject.h"

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

		HPrefab prefab = gResources().load<Prefab>(nativePath);
		HSceneObject root = prefab->instantiate();

		MonoString* uuid = MonoUtil::stringToMono(MonoManager::instance().getDomain(), prefab.getUUID());

		// TODO - Return actual prefab
		return uuid;
	}

	void ScriptScene::internal_ClearScene()
	{
		gSceneManager().clearScene();
	}
}
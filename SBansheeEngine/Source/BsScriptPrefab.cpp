#include "BsScriptPrefab.h"
#include "BsScriptResourceManager.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoArray.h"
#include "BsMonoManager.h"
#include "BsScriptSceneObject.h"

namespace BansheeEngine
{
	ScriptPrefab::ScriptPrefab(MonoObject* instance, const HPrefab& prefab)
		:ScriptObject(instance), mPrefab(prefab)
	{

	}

	void ScriptPrefab::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptPrefab::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetRoot", &ScriptPrefab::internal_GetRoot);
		metaData.scriptClass->addInternalCall("Internal_Instantiate", &ScriptPrefab::internal_Instantiate);
	}

	void ScriptPrefab::internal_CreateInstance(MonoObject* instance, ScriptSceneObject* so)
	{
		HPrefab prefab = Prefab::create(so->getNativeSceneObject());

		ScriptPrefab* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, prefab, &scriptInstance);
	}

	MonoObject* ScriptPrefab::internal_GetRoot(ScriptPrefab* thisPtr)
	{
		HPrefab prefab = thisPtr->getPrefabHandle();

		HSceneObject instance = prefab->getRoot();
		ScriptSceneObject* scriptInstance = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(instance);

		return scriptInstance->getManagedInstance();
	}

	MonoObject* ScriptPrefab::internal_Instantiate(ScriptPrefab* thisPtr)
	{
		HPrefab prefab = thisPtr->getPrefabHandle();

		HSceneObject instance = prefab->instantiate();
		ScriptSceneObject* scriptInstance = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(instance);

		return scriptInstance->getManagedInstance();
	}
		
	void ScriptPrefab::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptResourceManager::instance().destroyScriptResource(this);
	}

	void ScriptPrefab::setNativeHandle(const HResource& resource)
	{
		mPrefab = static_resource_cast<Prefab>(resource);
	}
}
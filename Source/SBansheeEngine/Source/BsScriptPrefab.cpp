//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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
		:TScriptResource(instance, prefab)
	{

	}

	void ScriptPrefab::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptPrefab::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_Instantiate", &ScriptPrefab::internal_Instantiate);
	}

	void ScriptPrefab::internal_CreateInstance(MonoObject* instance, ScriptSceneObject* so)
	{
		HPrefab prefab = Prefab::create(so->getNativeSceneObject());

		ScriptPrefab* scriptInstance;
		ScriptResourceManager::instance().createScriptResource(instance, prefab, &scriptInstance);
	}

	MonoObject* ScriptPrefab::internal_Instantiate(ScriptPrefab* thisPtr)
	{
		HPrefab prefab = thisPtr->getHandle();

		HSceneObject instance = prefab->instantiate();
		ScriptSceneObject* scriptInstance = ScriptGameObjectManager::instance().getOrCreateScriptSceneObject(instance);

		return scriptInstance->getManagedInstance();
	}

	MonoObject* ScriptPrefab::createInstance()
	{
		return metaData.scriptClass->createInstance();
	}
}
#include "BsScriptSceneObject.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSceneObject.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptSceneObject::ScriptSceneObject(MonoObject* instance, const HSceneObject& sceneObject)
		:ScriptObject(instance), mSceneObject(sceneObject)
	{

	}

	void ScriptSceneObject::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptSceneObject::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_GetParent", &ScriptSceneObject::internal_getParent);
		metaData.scriptClass->addInternalCall("Internal_SetParent", &ScriptSceneObject::internal_setParent);
		metaData.scriptClass->addInternalCall("Internal_GetNumChildren", &ScriptSceneObject::internal_getNumChildren);
		metaData.scriptClass->addInternalCall("Internal_GetChild", &ScriptSceneObject::internal_getChild);
	}

	void ScriptSceneObject::internal_createInstance(MonoObject* instance, MonoString* name)
	{
		HSceneObject sceneObject = SceneObject::create(toString(MonoUtil::monoToWString(name)));

		ScriptGameObjectManager::instance().createScriptSceneObject(instance, sceneObject);
	}

	void ScriptSceneObject::internal_setParent(ScriptSceneObject* nativeInstance, MonoObject* parent)
	{
		ScriptSceneObject* parentScriptSO = ScriptSceneObject::toNative(parent);

		nativeInstance->mSceneObject->setParent(parentScriptSO->mSceneObject);
	}

	MonoObject* ScriptSceneObject::internal_getParent(ScriptSceneObject* nativeInstance)
	{
		HSceneObject parent = nativeInstance->mSceneObject->getParent();

		ScriptSceneObject* parentScriptSO = ScriptGameObjectManager::instance().getScriptSceneObject(parent);
		if(parentScriptSO == nullptr)
			parentScriptSO = ScriptGameObjectManager::instance().createScriptSceneObject(parent);

		return parentScriptSO->getManagedInstance();
	}

	UINT32 ScriptSceneObject::internal_getNumChildren(ScriptSceneObject* nativeInstance)
	{
		return nativeInstance->mSceneObject->getNumChildren();
	}

	MonoObject* ScriptSceneObject::internal_getChild(ScriptSceneObject* nativeInstance, UINT32 idx)
	{
		UINT32 numChildren = nativeInstance->mSceneObject->getNumChildren();
		if(idx >= numChildren)
		{
			LOGWRN("Attempting to access an out of range SceneObject child. Provided index: \"" + toString(idx)
				+ "\". Valid range: [0 .. " + toString(numChildren) + ")");
			return nullptr;
		}

		HSceneObject childSO = nativeInstance->mSceneObject->getChild(idx);
		ScriptSceneObject* childScriptSO = ScriptGameObjectManager::instance().getScriptSceneObject(childSO);
		if(childScriptSO == nullptr)
			childScriptSO = ScriptGameObjectManager::instance().createScriptSceneObject(childSO);

		return childScriptSO->getManagedInstance();
	}

	void ScriptSceneObject::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;
		ScriptGameObjectManager::instance().destroyScriptGameObject(this);
	}

	void ScriptSceneObject::setNativeHandle(const HGameObject& gameObject)
	{
		mSceneObject = static_object_cast<SceneObject>(gameObject);
	}
}
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

		metaData.scriptClass->addInternalCall("Internal_GetPosition", &ScriptSceneObject::internal_getPosition);
		metaData.scriptClass->addInternalCall("Internal_GetLocalPosition", &ScriptSceneObject::internal_getLocalPosition);
		metaData.scriptClass->addInternalCall("Internal_GetRotation", &ScriptSceneObject::internal_getRotation);
		metaData.scriptClass->addInternalCall("Internal_GetLocalRotation", &ScriptSceneObject::internal_getLocalRotation);
		metaData.scriptClass->addInternalCall("Internal_GetScale", &ScriptSceneObject::internal_getScale);
		metaData.scriptClass->addInternalCall("Internal_GetLocalScale", &ScriptSceneObject::internal_getLocalScale);

		metaData.scriptClass->addInternalCall("Internal_SetPosition", &ScriptSceneObject::internal_setPosition);
		metaData.scriptClass->addInternalCall("Internal_SetLocalPosition", &ScriptSceneObject::internal_setLocalPosition);
		metaData.scriptClass->addInternalCall("Internal_SetRotation", &ScriptSceneObject::internal_setRotation);
		metaData.scriptClass->addInternalCall("Internal_SetLocalRotation", &ScriptSceneObject::internal_setLocalRotation);
		metaData.scriptClass->addInternalCall("Internal_SetLocalScale", &ScriptSceneObject::internal_setLocalScale);
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

	void ScriptSceneObject::internal_getPosition(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSceneObject->getWorldPosition();
	}

	void ScriptSceneObject::internal_getLocalPosition(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSceneObject->getPosition();
	}

	void ScriptSceneObject::internal_getRotation(ScriptSceneObject* nativeInstance, Quaternion* value)
	{
		*value = nativeInstance->mSceneObject->getWorldRotation();
	}

	void ScriptSceneObject::internal_getLocalRotation(ScriptSceneObject* nativeInstance, Quaternion* value)
	{
		*value = nativeInstance->mSceneObject->getRotation();
	}

	void ScriptSceneObject::internal_getScale(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSceneObject->getWorldScale();
	}

	void ScriptSceneObject::internal_getLocalScale(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSceneObject->getWorldScale();
	}

	void ScriptSceneObject::internal_setPosition(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		nativeInstance->mSceneObject->setWorldPosition(value);
	}

	void ScriptSceneObject::internal_setLocalPosition(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		nativeInstance->mSceneObject->setPosition(value);
	}

	void ScriptSceneObject::internal_setRotation(ScriptSceneObject* nativeInstance, Quaternion value)
	{
		nativeInstance->mSceneObject->setWorldRotation(value);
	}

	void ScriptSceneObject::internal_setLocalRotation(ScriptSceneObject* nativeInstance, Quaternion value)
	{
		nativeInstance->mSceneObject->setRotation(value);
	}

	void ScriptSceneObject::internal_setLocalScale(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		nativeInstance->mSceneObject->setScale(value);
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
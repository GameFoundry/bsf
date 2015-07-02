#include "BsScriptSceneObject.h"
#include "BsScriptGameObjectManager.h"
#include "BsScriptResourceManager.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsSceneObject.h"
#include "BsMonoUtil.h"
#include "BsScriptPrefab.h"
#include "BsPrefab.h"
#include "BsPrefabUtility.h"
#include "BsResources.h"

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

		metaData.scriptClass->addInternalCall("Internal_GetPrefab", &ScriptSceneObject::internal_getPrefab);
		metaData.scriptClass->addInternalCall("Internal_BreakPrefab", &ScriptSceneObject::internal_breakPrefab);
		metaData.scriptClass->addInternalCall("Internal_ApplyPrefab", &ScriptSceneObject::internal_applyPrefab);
		metaData.scriptClass->addInternalCall("Internal_RevertPrefab", &ScriptSceneObject::internal_revertPrefab);

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

		metaData.scriptClass->addInternalCall("Internal_GetLocalTransform", &ScriptSceneObject::internal_getLocalTransform);
		metaData.scriptClass->addInternalCall("Internal_GetWorldTransform", &ScriptSceneObject::internal_getWorldTransform);
		metaData.scriptClass->addInternalCall("Internal_LookAt", &ScriptSceneObject::internal_lookAt);
		metaData.scriptClass->addInternalCall("Internal_Move", &ScriptSceneObject::internal_move);
		metaData.scriptClass->addInternalCall("Internal_MoveLocal", &ScriptSceneObject::internal_moveLocal);
		metaData.scriptClass->addInternalCall("Internal_Rotate", &ScriptSceneObject::internal_rotate);
		metaData.scriptClass->addInternalCall("Internal_Roll", &ScriptSceneObject::internal_roll);
		metaData.scriptClass->addInternalCall("Internal_Yaw", &ScriptSceneObject::internal_yaw);
		metaData.scriptClass->addInternalCall("Internal_Pitch", &ScriptSceneObject::internal_pitch);
		metaData.scriptClass->addInternalCall("Internal_SetForward", &ScriptSceneObject::internal_setForward);
		metaData.scriptClass->addInternalCall("Internal_GetForward", &ScriptSceneObject::internal_getForward);
		metaData.scriptClass->addInternalCall("Internal_GetUp", &ScriptSceneObject::internal_getUp);
		metaData.scriptClass->addInternalCall("Internal_GetRight", &ScriptSceneObject::internal_getRight);

		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptSceneObject::internal_destroy);
	}

	void ScriptSceneObject::internal_createInstance(MonoObject* instance, MonoString* name)
	{
		HSceneObject sceneObject = SceneObject::create(toString(MonoUtil::monoToWString(name)));

		ScriptGameObjectManager::instance().createScriptSceneObject(instance, sceneObject);
	}

	void ScriptSceneObject::internal_setParent(ScriptSceneObject* nativeInstance, MonoObject* parent)
	{
		if (checkIfDestroyed(nativeInstance))
			return;

		ScriptSceneObject* parentScriptSO = ScriptSceneObject::toNative(parent);

		nativeInstance->mSceneObject->setParent(parentScriptSO->mSceneObject);
	}

	MonoObject* ScriptSceneObject::internal_getParent(ScriptSceneObject* nativeInstance)
	{
		if (checkIfDestroyed(nativeInstance))
			return nullptr;

		HSceneObject parent = nativeInstance->mSceneObject->getParent();

		ScriptSceneObject* parentScriptSO = ScriptGameObjectManager::instance().getScriptSceneObject(parent);
		if(parentScriptSO == nullptr)
			parentScriptSO = ScriptGameObjectManager::instance().createScriptSceneObject(parent);

		return parentScriptSO->getManagedInstance();
	}

	void ScriptSceneObject::internal_getNumChildren(ScriptSceneObject* nativeInstance, UINT32* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getNumChildren();
		else
			*value = 0;
	}

	MonoObject* ScriptSceneObject::internal_getChild(ScriptSceneObject* nativeInstance, UINT32 idx)
	{
		if (checkIfDestroyed(nativeInstance))
			return nullptr;

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

	MonoObject* ScriptSceneObject::internal_getPrefab(ScriptSceneObject* nativeInstance)
	{
		if (checkIfDestroyed(nativeInstance))
			return nullptr;

		String prefabLinkUUID = nativeInstance->mSceneObject->getPrefabLink();
		HPrefab prefab = static_resource_cast<Prefab>(gResources().loadFromUUID(prefabLinkUUID, false, false));

		if (prefab != nullptr)
		{
			ScriptPrefab* scriptPrefab = ScriptResourceManager::instance().getScriptPrefab(prefab);
			if (scriptPrefab == nullptr)
				scriptPrefab = ScriptResourceManager::instance().createScriptPrefab(prefab);

			return scriptPrefab->getManagedInstance();
		}

		return nullptr;
	}

	void ScriptSceneObject::internal_breakPrefab(ScriptSceneObject* nativeInstance)
	{
		if (checkIfDestroyed(nativeInstance))
			return;

		nativeInstance->mSceneObject->breakPrefabLink();
	}

	void ScriptSceneObject::internal_applyPrefab(ScriptSceneObject* nativeInstance)
	{
		if (checkIfDestroyed(nativeInstance))
			return;

		String prefabLinkUUID = nativeInstance->mSceneObject->getPrefabLink();
		HPrefab prefab = static_resource_cast<Prefab>(gResources().loadFromUUID(prefabLinkUUID, false, false));

		if (prefab != nullptr)
			prefab->update(nativeInstance->mSceneObject);
	}

	void ScriptSceneObject::internal_revertPrefab(ScriptSceneObject* nativeInstance)
	{
		if (checkIfDestroyed(nativeInstance))
			return;

		PrefabUtility::revertToPrefab(nativeInstance->mSceneObject);
	}

	void ScriptSceneObject::internal_getPosition(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getWorldPosition();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_getLocalPosition(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getPosition();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_getRotation(ScriptSceneObject* nativeInstance, Quaternion* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getWorldRotation();
		else
			*value = Quaternion();
	}

	void ScriptSceneObject::internal_getLocalRotation(ScriptSceneObject* nativeInstance, Quaternion* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getRotation();
		else
			*value = Quaternion();
	}

	void ScriptSceneObject::internal_getScale(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getWorldScale();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_getLocalScale(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getWorldScale();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_setPosition(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setWorldPosition(value);
	}

	void ScriptSceneObject::internal_setLocalPosition(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setPosition(value);
	}

	void ScriptSceneObject::internal_setRotation(ScriptSceneObject* nativeInstance, Quaternion value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setWorldRotation(value);
	}

	void ScriptSceneObject::internal_setLocalRotation(ScriptSceneObject* nativeInstance, Quaternion value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setRotation(value);
	}

	void ScriptSceneObject::internal_setLocalScale(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setScale(value);
	}

	void ScriptSceneObject::internal_getLocalTransform(ScriptSceneObject* nativeInstance, Matrix4* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getLocalTfrm();
		else
			*value = Matrix4();
	}

	void ScriptSceneObject::internal_getWorldTransform(ScriptSceneObject* nativeInstance, Matrix4* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getWorldTfrm();
		else
			*value = Matrix4();
	}

	void ScriptSceneObject::internal_lookAt(ScriptSceneObject* nativeInstance, Vector3 direction, Vector3 up)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->lookAt(direction, up);
	}

	void ScriptSceneObject::internal_move(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->move(value);
	}

	void ScriptSceneObject::internal_moveLocal(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->moveRelative(value);
	}

	void ScriptSceneObject::internal_rotate(ScriptSceneObject* nativeInstance, Quaternion value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->rotate(value);
	}

	void ScriptSceneObject::internal_roll(ScriptSceneObject* nativeInstance, Radian value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->roll(value);
	}

	void ScriptSceneObject::internal_yaw(ScriptSceneObject* nativeInstance, Radian value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->yaw(value);
	}

	void ScriptSceneObject::internal_pitch(ScriptSceneObject* nativeInstance, Radian value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->pitch(value);
	}

	void ScriptSceneObject::internal_setForward(ScriptSceneObject* nativeInstance, Vector3 value)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->setForward(value);
	}

	void ScriptSceneObject::internal_getForward(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getForward();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_getUp(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getUp();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_getRight(ScriptSceneObject* nativeInstance, Vector3* value)
	{
		if (!checkIfDestroyed(nativeInstance))
			*value = nativeInstance->mSceneObject->getRight();
		else
			*value = Vector3();
	}

	void ScriptSceneObject::internal_destroy(ScriptSceneObject* nativeInstance, bool immediate)
	{
		if (!checkIfDestroyed(nativeInstance))
			nativeInstance->mSceneObject->destroy(immediate);
	}

	bool ScriptSceneObject::checkIfDestroyed(ScriptSceneObject* nativeInstance)
	{
		if (nativeInstance->mSceneObject.isDestroyed())
		{
			LOGWRN("Trying to access a destroyed SceneObject with instance ID: " + nativeInstance->mSceneObject.getInstanceId());
			return true;
		}

		return false;
	}

	void ScriptSceneObject::_onManagedInstanceDeleted()
	{
		mManagedInstance = nullptr;

		if (!mRefreshInProgress)
			ScriptGameObjectManager::instance().destroyScriptGameObject(this);
	}

	void ScriptSceneObject::setNativeHandle(const HGameObject& gameObject)
	{
		mSceneObject = static_object_cast<SceneObject>(gameObject);
	}
}
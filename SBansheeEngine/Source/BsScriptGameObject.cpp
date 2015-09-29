#include "BsScriptGameObject.h"
#include "BsScriptGameObjectManager.h"

namespace BansheeEngine
{
	ScriptGameObjectBase::ScriptGameObjectBase(MonoObject* instance)
		:PersistentScriptObjectBase(instance), mRefreshInProgress(false)
	{
		
	}

	ScriptObjectBackup ScriptGameObjectBase::beginRefresh()
	{
		mRefreshInProgress = true;

		return PersistentScriptObjectBase::beginRefresh();
	}

	void ScriptGameObjectBase::endRefresh(const ScriptObjectBackup& backupData)
	{
		mRefreshInProgress = false;

		PersistentScriptObjectBase::endRefresh(backupData);
	}

	ScriptGameObject::ScriptGameObject(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptGameObject::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetInstanceId", &ScriptGameObject::internal_getInstanceId);
	}

	UINT64 ScriptGameObject::internal_getInstanceId(ScriptGameObject* nativeInstance)
	{
		return nativeInstance->getNativeHandle().getInstanceId();
	}
}
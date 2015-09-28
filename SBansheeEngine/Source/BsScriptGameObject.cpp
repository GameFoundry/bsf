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
		mRefreshInProgress.store(true, std::memory_order_release);

		return PersistentScriptObjectBase::beginRefresh();
	}

	void ScriptGameObjectBase::endRefresh(const ScriptObjectBackup& backupData)
	{
		mRefreshInProgress.store(false, std::memory_order_release);

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
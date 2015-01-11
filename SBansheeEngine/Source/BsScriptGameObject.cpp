#include "BsScriptGameObject.h"

namespace BansheeEngine
{
	ScriptGameObjectBase::ScriptGameObjectBase(MonoObject* instance)
		:PersistentScriptObjectBase(instance), mRefreshInProgress(false)
	{ }

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
}
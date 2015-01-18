#include "BsScriptObjectManager.h"
#include "BsScriptObject.h"
#include "BsMonoManager.h"
#include "BsScriptAssemblyManager.h"

namespace BansheeEngine
{
	ScriptObjectManager::ScriptObjectManager()
	{

	}

	void ScriptObjectManager::registerScriptObject(ScriptObjectBase* instance)
	{
		mScriptObjects.insert(instance);
	}

	void ScriptObjectManager::unregisterScriptObject(ScriptObjectBase* instance)
	{
		mScriptObjects.erase(instance);
	}

	void ScriptObjectManager::refreshAssemblies()
	{
		Map<ScriptObjectBase*, ScriptObjectBackup> backupData;

		onRefreshStarted();

		for (auto& scriptObject : mScriptObjects)
			backupData[scriptObject] = scriptObject->beginRefresh();

		MonoManager::instance().unloadScriptDomain();

		for (auto& scriptObject : mScriptObjects)
			assert(scriptObject->isPersistent() && "Non-persistent ScriptObject alive after domain unload.");

		for (auto& scriptObject : mScriptObjects)
			scriptObject->_clearManagedInstance();

		MonoManager::instance().loadScriptDomain();
		ScriptAssemblyManager::instance().refreshAssemblyInfo();

		onRefreshDomainLoaded();

		for (auto& scriptObject : mScriptObjects)
			scriptObject->_restoreManagedInstance();

		for (auto& scriptObject : mScriptObjects)
			scriptObject->endRefresh(backupData[scriptObject]);

		onRefreshComplete();
	}
}
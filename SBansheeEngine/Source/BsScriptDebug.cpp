#include "BsScriptDebug.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	HEvent ScriptDebug::mOnLogEntryAddedConn;
	ScriptDebug::OnAddedThunkDef ScriptDebug::onAddedThunk = nullptr;

	ScriptDebug::ScriptDebug(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptDebug::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_Log", &ScriptDebug::internal_log);
		metaData.scriptClass->addInternalCall("Internal_LogWarning", &ScriptDebug::internal_logWarning);
		metaData.scriptClass->addInternalCall("Internal_LogError", &ScriptDebug::internal_logError);

		onAddedThunk = (OnAddedThunkDef)metaData.scriptClass->getMethod("Internal_OnAdded", 2)->getThunk();
	}

	void ScriptDebug::startUp()
	{
		mOnLogEntryAddedConn = gDebug().onLogEntryAdded.connect(&ScriptDebug::onLogEntryAdded);
	}

	void ScriptDebug::shutDown()
	{
		mOnLogEntryAddedConn.disconnect();
	}

	void ScriptDebug::onLogEntryAdded(const LogEntry& entry)
	{
		MonoString* message = MonoUtil::stringToMono(MonoManager::instance().getDomain(), entry.getMessage());

		MonoUtil::invokeThunk(onAddedThunk, entry.getChannel(), message);
	}

	void ScriptDebug::internal_log(MonoString* message)
	{
		gDebug().logDebug(MonoUtil::monoToString(message));
	}

	void ScriptDebug::internal_logWarning(MonoString* message)
	{
		gDebug().logWarning(MonoUtil::monoToString(message));
	}

	void ScriptDebug::internal_logError(MonoString* message)
	{
		gDebug().logError(MonoUtil::monoToString(message));
	}
}
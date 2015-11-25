#include "BsScriptLogEntry.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptLogEntry::ScriptLogEntry(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptLogEntry::initRuntimeData()
	{ }
}
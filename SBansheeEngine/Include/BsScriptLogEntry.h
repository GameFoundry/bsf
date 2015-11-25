#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for LogEntry.
	 */
	class BS_SCR_BE_EXPORT ScriptLogEntry : public ScriptObject <ScriptLogEntry>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "LogEntry")

	private:
		ScriptLogEntry(MonoObject* instance);
	};
}
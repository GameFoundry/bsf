#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptDebug : public ScriptObject<ScriptDebug>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Debug")

	private:
		static void internal_log(MonoString* message);
		static void internal_logWarning(MonoString* message);
		static void internal_logError(MonoString* message);

		ScriptDebug(MonoObject* instance);
	};
}
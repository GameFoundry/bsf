#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsStringTable.h"
#include "BsStringTableManager.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptStringTableManager : public ScriptObject <ScriptStringTableManager>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTables")

	private:
		static void internal_GetActiveLanguage(Language* value);
		static void internal_SetActiveLanguage(Language value);

		static MonoObject* internal_GetTable(UINT32 id);
		static void internal_SetTable(UINT32 id, MonoObject* table);
		static void internal_RemoveTable(UINT32 id);

		ScriptStringTableManager(MonoObject* instance);
	};
}
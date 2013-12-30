#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmStringTable.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptStringTable : public ScriptObject<ScriptStringTable>
	{
	public:
		static void initMetaData();

	private:
		static void internal_GetActiveLanguage(CM::Language* value);
		static void internal_SetActiveLanguage(CM::Language value);

		static void internal_SetString(MonoString* identifier, CM::Language language, MonoString* value);
		static void internal_RemoveString(MonoString* identifier);
		static void internal_GetLocalizedString(MonoString* identifier, MonoString** value);

		static void initRuntimeData();

		ScriptStringTable() { }
	};
}
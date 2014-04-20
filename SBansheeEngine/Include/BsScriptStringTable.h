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
		static void internal_GetActiveLanguage(Language* value);
		static void internal_SetActiveLanguage(Language value);

		static void internal_SetString(MonoString* identifier, Language language, MonoString* value);
		static void internal_RemoveString(MonoString* identifier);
		static void internal_GetLocalizedString(MonoString* identifier, MonoString** value);

		static void initRuntimeData();

		ScriptStringTable() { }
	};
}
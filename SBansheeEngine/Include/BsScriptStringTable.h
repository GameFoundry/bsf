#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for StringTable.
	 */
	class BS_SCR_BE_EXPORT ScriptStringTable : public TScriptResource <ScriptStringTable, StringTable>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTable")

		ScriptStringTable(MonoObject* instance, const HStringTable& table);

	private:
		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance);
		static bool internal_Contains(ScriptStringTable* thisPtr, MonoString* identifier);

		static UINT32 internal_GetNumStrings(ScriptStringTable* thisPtr);
		static MonoArray* internal_GetIdentifiers(ScriptStringTable* thisPtr);

		static void internal_SetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString* value);
		static void internal_SetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString* value);

		static void internal_RemoveString(ScriptStringTable* thisPtr, MonoString* identifier);

		static void internal_GetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString** value);
		static void internal_GetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString** value);
	};
}
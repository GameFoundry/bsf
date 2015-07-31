#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptStringTable : public ScriptObject <ScriptStringTable, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTable")

		ScriptStringTable(MonoObject* instance, const HStringTable& table);

		HResource getNativeHandle() const override { return mTable; }
		void setNativeHandle(const HResource& resource) override;

		HStringTable getStringTableHandle() const { return mTable; }

	private:
		static UINT32 internal_GetNumStrings(ScriptStringTable* thisPtr);
		static MonoArray* internal_GetIdentifiers(ScriptStringTable* thisPtr);

		static void internal_SetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString* value);
		static void internal_SetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString* value);

		static void internal_RemoveString(ScriptStringTable* thisPtr, MonoString* identifier);

		static void internal_GetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString** value);
		static void internal_GetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString** value);

		void _onManagedInstanceDeleted() override;

		HStringTable mTable;
	};
}
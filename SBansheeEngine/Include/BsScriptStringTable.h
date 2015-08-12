#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsStringTable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for StringTable.
	 */
	class BS_SCR_BE_EXPORT ScriptStringTable : public ScriptObject <ScriptStringTable, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "StringTable")

		ScriptStringTable(MonoObject* instance, const HStringTable& table);

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mTable; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal string table resource.
		 */
		HStringTable getStringTableHandle() const { return mTable; }

	private:
		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HStringTable mTable;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance);

		static UINT32 internal_GetNumStrings(ScriptStringTable* thisPtr);
		static MonoArray* internal_GetIdentifiers(ScriptStringTable* thisPtr);

		static void internal_SetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString* value);
		static void internal_SetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString* value);

		static void internal_RemoveString(ScriptStringTable* thisPtr, MonoString* identifier);

		static void internal_GetString(ScriptStringTable* thisPtr, MonoString* identifier, Language language, MonoString** value);
		static void internal_GetStringDefault(ScriptStringTable* thisPtr, MonoString* identifier, MonoString** value);
	};
}
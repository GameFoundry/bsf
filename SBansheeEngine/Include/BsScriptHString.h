#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsHString.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptHString : public ScriptObject<ScriptHString>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "LocString")

		const HString& getInternalValue() const { return mString; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* identifier, UINT32 tableId);
		static void internal_setParameter(HString* nativeInstance, UINT32 idx, MonoString* value);
		static void internal_getValue(HString* nativeInstance, MonoString** value);

		ScriptHString(MonoObject* instance, const HString& string);

		HString mString;
	};
}
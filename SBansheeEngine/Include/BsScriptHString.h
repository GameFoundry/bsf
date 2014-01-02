#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmHString.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptHString : public ScriptObject<ScriptHString>
	{
	public:
		static void initMetaData();

		const CM::HString& getInternalValue() const { return mString; }
		void* getNativeRaw() const { return (void*)&mString; }

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* identifier);
		static void internal_destroyInstance(ScriptHString* nativeInstance);
		static void internal_setParameter(CM::HString* nativeInstance, CM::UINT32 idx, MonoString* value);
		static void internal_getValue(CM::HString* nativeInstance, MonoString** value);

		static void initRuntimeData();

		ScriptHString(const CM::HString& string);

		GUIArea* mArea;
		CM::HString mString;
	};
}
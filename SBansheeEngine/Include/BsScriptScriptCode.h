#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "BsScriptCode.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptScriptCode : public ScriptObject <ScriptScriptCode, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ScriptCode")

		void* getNativeRaw() const { return mScriptCode.get(); }

		HResource getNativeHandle() const { return mScriptCode; }
		void setNativeHandle(const HResource& resource);

		HScriptCode getScriptCodeHandle() const { return mScriptCode; }
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, MonoString* text);
		static MonoString* internal_getText(ScriptScriptCode* thisPtr);
		static void internal_setText(ScriptScriptCode* thisPtr, MonoString* text);

		ScriptScriptCode(MonoObject* instance, const HScriptCode& scriptCode);

		void _onManagedInstanceDeleted();

		HScriptCode mScriptCode;
	};
}
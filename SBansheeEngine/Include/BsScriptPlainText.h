#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "BsPlainText.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptPlainText : public ScriptObject <ScriptPlainText, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "PlainText")

		void* getNativeRaw() const { return mPlainText.get(); }

		HResource getNativeHandle() const { return mPlainText; }
		void setNativeHandle(const HResource& resource);

		HPlainText getPlainTextHandle() const { return mPlainText; }
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, MonoString* text);
		static MonoString* internal_getText(ScriptPlainText* thisPtr);
		static void internal_setText(ScriptPlainText* thisPtr, MonoString* text);

		ScriptPlainText(MonoObject* instance, const HPlainText& plainText);

		void _onManagedInstanceDeleted();

		HPlainText mPlainText;
	};
}
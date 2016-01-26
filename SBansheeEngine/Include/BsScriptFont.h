#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptFont : public ScriptObject<ScriptFont>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Font")

		void* getNativeRaw() const;
		const HFont& getInternalValue() const { return mFont; }

	private:
		static void internal_createInstanceExternal(MonoObject* instance, const HFont& font);

		ScriptFont(MonoObject* instance, const HFont& font);

		HFont mFont;
	};
}
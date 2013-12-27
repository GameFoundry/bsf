#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptFont : public ScriptObject<ScriptFont>
	{
	public:
		static void initMetaData();

		void* getNativeRaw() const;
		const CM::HFont& getInternalValue() const { return mFont; }

	private:
		static void internal_createInstanceExternal(MonoObject* instance, const CM::HFont& font);
		static void internal_destroyInstance(ScriptFont* nativeInstance);

		static void initRuntimeData();

		ScriptFont(const CM::HFont& font);

		CM::HFont mFont;
	};
}
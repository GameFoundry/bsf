#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsColor.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptColor : public ScriptObject <ScriptColor>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Color")

		static Color unbox(MonoObject* obj);
		static MonoObject* box(const Color& value);

	private:
		ScriptColor(MonoObject* instance);
	};
}
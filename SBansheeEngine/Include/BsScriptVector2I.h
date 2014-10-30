#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptVector2I : public ScriptObject<ScriptVector2I>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Vector2I")

		static Vector2I unbox(MonoObject* obj);
		static MonoObject* box(const Vector2I& value);

	private:
		ScriptVector2I(MonoObject* instance);
	};
}
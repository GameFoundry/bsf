#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptScene : public ScriptObject<ScriptScene>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Scene")

	private:
		static MonoString* internal_LoadScene(MonoString* path);
		static void internal_ClearScene();

		ScriptScene(MonoObject* instance);
	};
}
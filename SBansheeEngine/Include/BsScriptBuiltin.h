#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptBuiltin : public ScriptObject <ScriptBuiltin>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Builtin")

	private:
		static MonoObject* internal_getWhiteTexture();
		static MonoObject* internal_getDiffuseShader();
		
		ScriptBuiltin(MonoObject* instance);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for BuiltinResources.
	 */
	class BS_SCR_BE_EXPORT ScriptBuiltin : public ScriptObject <ScriptBuiltin>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Builtin")

	private:
		ScriptBuiltin(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static MonoObject* internal_getWhiteTexture();
		static MonoObject* internal_getDiffuseShader();
	};
}
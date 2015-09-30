#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsShader.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for Shader.
	 */
	class BS_SCR_BE_EXPORT ScriptShader : public TScriptResource <ScriptShader, Shader>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Shader")

	private:
		friend class ScriptResourceManager;

		ScriptShader(MonoObject* instance, const HShader& shader);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_GetShaderParameters(ScriptShader* nativeInstance, MonoArray** outNames, MonoArray** outTypes);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptResource.h"
#include "../../../Extensions/BsShaderEx.h"

namespace bs
{
	class Shader;
	class ShaderEx;
	struct __ShaderParameterInterop;

	class BS_SCR_BE_EXPORT ScriptShader : public TScriptResource<ScriptShader, Shader>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "Shader")

		ScriptShader(MonoObject* managedInstance, const ResourceHandle<Shader>& value);

		static MonoObject* createInstance();

	private:
		static MonoObject* Internal_getRef(ScriptShader* thisPtr);

		static MonoArray* Internal_getParameters(ScriptShader* thisPtr);
	};
}

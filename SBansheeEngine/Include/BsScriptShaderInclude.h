#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsShaderInclude.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for ShaderInclude.
	 */
	class BS_SCR_BE_EXPORT ScriptShaderInclude : public TScriptResource <ScriptShaderInclude, ShaderInclude>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "ShaderInclude")

	private:
		friend class ScriptResourceManager;

		ScriptShaderInclude(MonoObject* instance, const HShaderInclude& shaderInclude);
	};
}
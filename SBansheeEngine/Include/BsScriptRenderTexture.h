#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptRenderTarget.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for RenderTexture.
	 */
	class BS_SCR_BE_EXPORT ScriptRenderTexture : public ScriptObject <ScriptRenderTexture, ScriptRenderTargetBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "RenderTexture")

	private:
		ScriptRenderTexture(MonoObject* instance);
	};
}
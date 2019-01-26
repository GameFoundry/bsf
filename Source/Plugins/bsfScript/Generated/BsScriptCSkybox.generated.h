#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"

namespace bs
{
	class CSkybox;

	class BS_SCR_BE_EXPORT ScriptCSkybox : public TScriptComponent<ScriptCSkybox, CSkybox>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "Skybox")

		ScriptCSkybox(MonoObject* managedInstance, const GameObjectHandle<CSkybox>& value);

	private:
		static MonoObject* Internal_getTexture(ScriptCSkybox* thisPtr);
		static void Internal_setTexture(ScriptCSkybox* thisPtr, MonoObject* texture);
		static void Internal_setBrightness(ScriptCSkybox* thisPtr, float brightness);
		static float Internal_getBrightness(ScriptCSkybox* thisPtr);
	};
}

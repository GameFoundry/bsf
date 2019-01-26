#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/BsScriptComponent.h"

namespace bs
{
	class CAudioListener;

	class BS_SCR_BE_EXPORT ScriptCAudioListener : public TScriptComponent<ScriptCAudioListener, CAudioListener>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "bs", "AudioListener")

		ScriptCAudioListener(MonoObject* managedInstance, const GameObjectHandle<CAudioListener>& value);

	private:
	};
}

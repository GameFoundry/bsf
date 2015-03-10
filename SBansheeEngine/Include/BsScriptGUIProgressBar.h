#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIProgressBar : public TScriptGUIElement <ScriptGUIProgressBar>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIProgressBar")

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_setPercent(ScriptGUIProgressBar* nativeInstance, float percent);
		static float internal_getPercent(ScriptGUIProgressBar* nativeInstance);
		static void internal_setTint(ScriptGUIProgressBar* nativeInstance, Color color);

		ScriptGUIProgressBar(MonoObject* instance, GUIProgressBar* progressBar);
	};
}
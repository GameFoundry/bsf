#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILabel : public TScriptGUIElement<ScriptGUILabel>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUILabel")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUILabel* nativeInstance, MonoObject* content);
		static void internal_setTint(ScriptGUILabel* nativeInstance, Color color);

		ScriptGUILabel(MonoObject* instance, GUILabel* label);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptGUIElement.h"
#include "BsVector2I.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUILayoutUtility : public ScriptObject<ScriptGUILayoutUtility>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "GUILayoutUtility")

		ScriptGUILayoutUtility();

	private:
		static void internal_CalculateOptimalSize(ScriptGUIElementBaseTBase* guiElement, Vector2I* output);
		static void internal_CalculateBounds(ScriptGUIElementBaseTBase* guiElement, Rect2I* output);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUIScrollArea.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIScrollArea : public TScriptGUIElement<ScriptGUIScrollArea>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIScrollArea")

	private:
		static void internal_createInstance(MonoObject* instance, ScrollBarType vertBarType, ScrollBarType horzBarType, 
			MonoString* scrollBarStyle, MonoString* scrollAreaStyle, MonoArray* guiOptions);
		static Rect2I internal_getContentBounds(ScriptGUIScrollArea* nativeInstance);
		static float internal_getHorzScroll(ScriptGUIScrollArea* nativeInstance);
		static void internal_setHorzScroll(ScriptGUIScrollArea* nativeInstance, float value);
		static float internal_getVertScroll(ScriptGUIScrollArea* nativeInstance);
		static void internal_setVertScroll(ScriptGUIScrollArea* nativeInstance, float value);

		ScriptGUIScrollArea(MonoObject* instance, GUIScrollArea* scrollArea);
	};
}
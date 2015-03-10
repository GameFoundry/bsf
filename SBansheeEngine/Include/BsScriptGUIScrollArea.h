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

		ScriptGUIScrollArea(MonoObject* instance, GUIScrollArea* scrollArea);
	};
}
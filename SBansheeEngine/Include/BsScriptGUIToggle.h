#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGUIToggle : public TScriptGUIElement<ScriptGUIToggle>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIToggle")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, 
			MonoObject* toggleGroup, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIToggle* nativeInstance, MonoObject* content);
		static void internal_toggleOn(ScriptGUIToggle* nativeInstance);
		static void internal_toggleOff(ScriptGUIToggle* nativeInstance);
		static void internal_setTint(ScriptGUIToggle* nativeInstance, Color color);

		static void onClick(MonoObject* instance);
		static void onHover(MonoObject* instance);
		static void onOut(MonoObject* instance);
		static void onToggled(MonoObject* instance, bool toggled);

		ScriptGUIToggle(MonoObject* instance, GUIToggle* toggle);

		typedef void (__stdcall *OnClickThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnHoverThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnOutThunkDef) (MonoObject*, MonoException**);
		typedef void (__stdcall *OnToggledThunkDef) (MonoObject*, bool toggled, MonoException**);

		static OnClickThunkDef onClickThunk;
		static OnHoverThunkDef onHoverThunk;
		static OnOutThunkDef onOutThunk;
		static OnToggledThunkDef onToggledThunk;
	};
}
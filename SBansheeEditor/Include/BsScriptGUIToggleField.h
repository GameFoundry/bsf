#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIToggleField : public TScriptGUIElement<ScriptGUIToggleField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIToggleField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIToggleField* nativeInstance, bool* output);
		static void internal_setValue(ScriptGUIToggleField* nativeInstance, bool value);
		static void internal_setTint(ScriptGUIToggleField* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, bool newValue);

		ScriptGUIToggleField(MonoObject* instance, GUIToggleField* toggleField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, bool, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
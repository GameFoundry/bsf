#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIIntField : public TScriptGUIElement<ScriptGUIIntField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIIntField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth, 
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIIntField* nativeInstance, INT32* output);
		static void internal_setValue(ScriptGUIIntField* nativeInstance, INT32 value);
		static void internal_hasInputFocus(ScriptGUIIntField* nativeInstance, bool* output);
		static void internal_setRange(ScriptGUIIntField* nativeInstance, INT32 min, INT32 max);
		static void internal_setTint(ScriptGUIIntField* nativeInstance, Color color);

		static void onChanged(MonoObject* instance, INT32 newValue);

		ScriptGUIIntField(MonoObject* instance, GUIIntField* intField);

		typedef void (__stdcall *OnChangedThunkDef) (MonoObject*, INT32, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUITextField : public TScriptGUIElement<ScriptGUITextField>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUITextField")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUITextField* nativeInstance, MonoString** output);
		static void internal_setValue(ScriptGUITextField* nativeInstance, MonoString* value);
		static void internal_hasInputFocus(ScriptGUITextField* nativeInstance, bool* output);

		static void onChanged(MonoObject* instance, const WString& newValue);

		ScriptGUITextField(MonoObject* instance, GUITextField* textField);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoString*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
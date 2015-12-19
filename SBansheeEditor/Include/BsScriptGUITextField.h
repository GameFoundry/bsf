#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUITextField.
	 */
	class BS_SCR_BED_EXPORT ScriptGUITextField : public TScriptGUIElement<ScriptGUITextField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUITextField")

	private:
		/**
		 * @brief	Triggered when the value in the native text field changes.
		 *
		 * @param	instance	Managed GUITextField instance.
		 * @param	newValue	New string value.
		 */
		static void onChanged(MonoObject* instance, const WString& newValue);

		/**
		 * @brief	Triggered when the user confirms input in the native text field.
		 *
		 * @param	instance	Managed GUITextField instance.
		 */
		static void onConfirmed(MonoObject* instance);

		ScriptGUITextField(MonoObject* instance, GUITextField* textField);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, bool multiline, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUITextField* nativeInstance, MonoString** output);
		static void internal_setValue(ScriptGUITextField* nativeInstance, MonoString* value);
		static void internal_hasInputFocus(ScriptGUITextField* nativeInstance, bool* output);
		static void internal_setTint(ScriptGUITextField* nativeInstance, Color* color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, MonoString*, MonoException**);
		typedef void(__stdcall *OnConfirmedThunkDef) (MonoObject*, MonoException**);

		static OnChangedThunkDef onChangedThunk;
		static OnConfirmedThunkDef onConfirmedThunk;
	};
}
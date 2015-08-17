#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsColor.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIColorField.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIColorField : public TScriptGUIElement<ScriptGUIColorField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIColorField")

	private:
		ScriptGUIColorField(MonoObject* instance, GUIColorField* colorField);

		/**
		 * @brief	Triggered when the value in the native color field changes.
		 *
		 * @param	instance	Managed GUIColorField instance.
		 * @param	newValue	New color value.
		 */
		static void onChanged(MonoObject* instance, Color newValue);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIColorField* nativeInstance, Color* output);
		static void internal_setValue(ScriptGUIColorField* nativeInstance, Color value);
		static void internal_setTint(ScriptGUIColorField* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Color, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIFloatField.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIFloatField : public TScriptGUIElement<ScriptGUIFloatField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIFloatField")

	private:
		ScriptGUIFloatField(MonoObject* instance, GUIFloatField* floatField);

		/**
		 * @brief	Triggered when the value in the native float field changes.
		 *
		 * @param	instance	Managed GUIFloatField instance.
		 * @param	newValue	New field value.
		 */
		static void onChanged(MonoObject* instance, float newValue);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIFloatField* nativeInstance, float* output);
		static void internal_setValue(ScriptGUIFloatField* nativeInstance, float value);
		static void internal_hasInputFocus(ScriptGUIFloatField* nativeInstance, bool* output);
		static void internal_setTint(ScriptGUIFloatField* nativeInstance, Color color);
		static void internal_setRange(ScriptGUIFloatField* nativeInstance, float min, float max);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, float, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
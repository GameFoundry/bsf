#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIVector2Field.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIVector2Field : public TScriptGUIElement<ScriptGUIVector2Field>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIVector2Field")

	private:
		/**
		 * @brief	Triggered when the value in the native vector field changes.
		 *
		 * @param	instance	Managed GUIVector2Field instance.
		 * @param	newValue	New vector value.
		 */
		static void onChanged(MonoObject* instance, Vector2 newValue);

		ScriptGUIVector2Field(MonoObject* instance, GUIVector2Field* vector2Field);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIVector2Field* nativeInstance, Vector2* output);
		static void internal_setValue(ScriptGUIVector2Field* nativeInstance, Vector2 value);
		static void internal_hasInputFocus(ScriptGUIVector2Field* nativeInstance, bool* output);
		static void internal_setTint(ScriptGUIVector2Field* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Vector2, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
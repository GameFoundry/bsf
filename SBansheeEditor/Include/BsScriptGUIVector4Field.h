#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIVector4Field.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIVector4Field : public TScriptGUIElement<ScriptGUIVector4Field>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIVector4Field")

	private:
		/**
		 * @brief	Triggered when the value in the native vector field changes.
		 *
		 * @param	instance	Managed GUIVector4Field instance.
		 * @param	newValue	New vector value.
		 */
		static void onChanged(MonoObject* instance, Vector4 newValue);

		ScriptGUIVector4Field(MonoObject* instance, GUIVector4Field* vector4Field);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* title, UINT32 titleWidth,
			MonoString* style, MonoArray* guiOptions, bool withTitle);

		static void internal_getValue(ScriptGUIVector4Field* nativeInstance, Vector4* output);
		static void internal_setValue(ScriptGUIVector4Field* nativeInstance, Vector4 value);
		static void internal_hasInputFocus(ScriptGUIVector4Field* nativeInstance, bool* output);
		static void internal_setTint(ScriptGUIVector4Field* nativeInstance, Color color);

		typedef void(__stdcall *OnChangedThunkDef) (MonoObject*, Vector4, MonoException**);

		static OnChangedThunkDef onChangedThunk;
	};
}
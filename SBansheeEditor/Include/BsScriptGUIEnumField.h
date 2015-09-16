#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIListBoxField, with some specific functionality
	 *			meant for displaying managed enumeration values.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIEnumField : public TScriptGUIElement<ScriptGUIEnumField>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIEnumField")

	private:
		/**
		 * @brief	Triggered when the value in the native list box selection changes.
		 *
		 * @param	instance	Managed GUIEnumField instance.
		 * @param	newIndex	New selection index.
		 */
		static void onSelectionChanged(MonoObject* instance, UINT32 newIndex);

		ScriptGUIEnumField(MonoObject* instance, GUIListBoxField* listBoxField, const Vector<UINT32>& values);

		Vector<UINT32> mValues;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoArray* names, MonoArray* values, MonoObject* title, 
			UINT32 titleWidth, MonoString* style, MonoArray* guiOptions, bool withTitle);

		static UINT32 internal_getValue(ScriptGUIEnumField* nativeInstance);
		static void internal_setValue(ScriptGUIEnumField* nativeInstance, UINT32 value);
		static void internal_setTint(ScriptGUIEnumField* nativeInstance, Color color);

		typedef void(__stdcall *OnSelectionChangedThunkDef) (MonoObject*, UINT32, MonoException**);

		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};
}
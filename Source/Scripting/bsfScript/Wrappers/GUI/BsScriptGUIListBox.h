//********************************* bs::framework - Copyright 2018-2019 Marko Pintera ************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "Wrappers/GUI/BsScriptGUIElement.h"
#include "GUI/BsGUITexture.h"

namespace bs
{
	/** @addtogroup ScriptInteropEngine
	 *  @{
	 */

	/**	Interop class between C++ & CLR for GUIListBox. */
	class BS_SCR_BE_EXPORT ScriptGUIListBox : public TScriptGUIElement<ScriptGUIListBox>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, ENGINE_NS, "GUIListBox")

	private:
		ScriptGUIListBox(MonoObject* instance, GUIListBox* listBox);

		/**	Triggered when the selected index in the native list box changes. */
		void onSelectionChanged(UINT32 index, bool enabled);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoArray* elements, bool multiselect, MonoString* style, MonoArray* guiOptions);
		static void internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements);
		static void internal_setTint(ScriptGUIListBox* nativeInstance, Color* color);
		static void internal_selectElement(ScriptGUIListBox* nativeInstance, int idx);
		static void internal_deselectElement(ScriptGUIListBox* nativeInstance, int idx);
		static MonoArray* internal_getElementStates(ScriptGUIListBox* nativeInstance);
		static void internal_setElementStates(ScriptGUIListBox* nativeInstance, MonoArray* states);

		typedef void (BS_THUNKCALL *OnSelectionChangedThunkDef) (MonoObject*, UINT32, MonoException**);
		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};

	/** @} */
}

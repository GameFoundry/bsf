#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUITexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIListBox. 
	 */
	class BS_SCR_BE_EXPORT ScriptGUIListBox : public TScriptGUIElement<ScriptGUIListBox>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GUIListBox")

	private:
		ScriptGUIListBox(MonoObject* instance, GUIListBox* listBox);

		/**
		 * @brief	Triggered when the selected index in the native list box changes.
		 */
		static void onSelectionChanged(MonoObject* instance, UINT32 index);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoArray* elements, MonoString* style, MonoArray* guiOptions);
		static void internal_setElements(ScriptGUIListBox* nativeInstance, MonoArray* elements);
		static void internal_setTint(ScriptGUIListBox* nativeInstance, Color color);

		typedef void (__stdcall *OnSelectionChangedThunkDef) (MonoObject*, UINT32, MonoException**);
		static OnSelectionChangedThunkDef onSelectionChangedThunk;
	};
}
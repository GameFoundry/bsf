#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIComponentFoldout.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIComponentFoldout : public TScriptGUIElement<ScriptGUIComponentFoldout>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIComponentFoldout")

	private:
		/**
		 * @brief	Triggered when the foldout is expanded or collapsed.
		 *
		 * @param	instance	Managed GUIComponentFoldout instance.
		 * @param	expanded	True if the foldout is now expanded, false otherwise.
		 */
		static void onToggled(MonoObject* instance, bool expanded);

		ScriptGUIComponentFoldout(MonoObject* instance, GUIComponentFoldout* foldout);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIComponentFoldout* nativeInstance, MonoObject* content);
		static void internal_setExpanded(ScriptGUIComponentFoldout* nativeInstance, bool expanded);
		static void internal_getIsExpanded(ScriptGUIComponentFoldout* nativeInstance, bool* isExpanded);
		static void internal_setTint(ScriptGUIComponentFoldout* nativeInstance, Color color);

		typedef void (__stdcall *OnToggledThunkDef) (MonoObject*, bool, MonoException**);

		static OnToggledThunkDef onToggledThunk;
	};
}
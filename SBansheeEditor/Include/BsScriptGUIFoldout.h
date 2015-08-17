#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUIFoldout.
	 */
	class BS_SCR_BED_EXPORT ScriptGUIFoldout : public TScriptGUIElement<ScriptGUIFoldout>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIFoldout")

	private:
		/**
		 * @brief	Triggered when the foldout is expanded or collapsed.
		 *
		 * @param	instance	Managed GUIFoldout instance.
		 * @param	expanded	True if the foldout is now expanded, false otherwise.
		 */
		static void onToggled(MonoObject* instance, bool expanded);

		ScriptGUIFoldout(MonoObject* instance, GUIFoldout* foldout);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIFoldout* nativeInstance, MonoObject* content);
		static void internal_setExpanded(ScriptGUIFoldout* nativeInstance, bool expanded);
		static void internal_getIsExpanded(ScriptGUIFoldout* nativeInstance, bool* isExpanded);
		static void internal_setTint(ScriptGUIFoldout* nativeInstance, Color color);

		typedef void(__stdcall *OnToggledThunkDef) (MonoObject*, bool, MonoException**);

		static OnToggledThunkDef onToggledThunk;
	};
}
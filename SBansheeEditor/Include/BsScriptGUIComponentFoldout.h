#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIComponentFoldout : public TScriptGUIElement<ScriptGUIComponentFoldout>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIComponentFoldout")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIComponentFoldout* nativeInstance, MonoObject* content);
		static void internal_setExpanded(ScriptGUIComponentFoldout* nativeInstance, bool expanded);
		static void internal_getIsExpanded(ScriptGUIComponentFoldout* nativeInstance, bool* isExpanded);
		static void internal_setTint(ScriptGUIComponentFoldout* nativeInstance, Color color);

		static void onToggled(MonoObject* instance, bool expanded);
		static void onRemoveClicked(MonoObject* instance);

		ScriptGUIComponentFoldout(MonoObject* instance, GUIComponentFoldout* foldout);

		typedef void (__stdcall *OnToggledThunkDef) (MonoObject*, bool, MonoException**);
		typedef void(__stdcall *OnRemoveClickedThunkDef) (MonoObject*, MonoException**);

		static OnToggledThunkDef onToggledThunk;
		static OnRemoveClickedThunkDef onRemoveClickedThunk;
	};
}
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIFoldout : public TScriptGUIElement<ScriptGUIFoldout>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIFoldout")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* content, MonoString* style, MonoArray* guiOptions);
		static void internal_setContent(ScriptGUIFoldout* nativeInstance, MonoObject* content);
		static void internal_setExpanded(ScriptGUIFoldout* nativeInstance, bool expanded);
		static void internal_getIsExpanded(ScriptGUIFoldout* nativeInstance, bool* isExpanded);

		static void onToggled(MonoObject* instance, bool expanded);

		ScriptGUIFoldout(MonoObject* instance, GUIFoldout* foldout);

		typedef void(__stdcall *OnToggledThunkDef) (MonoObject*, bool, MonoException**);

		static OnToggledThunkDef onToggledThunk;
	};
}
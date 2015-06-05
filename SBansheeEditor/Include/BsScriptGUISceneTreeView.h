#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUISceneTreeView : public TScriptGUIElement<ScriptGUISceneTreeView>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUISceneTreeView")

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_update(ScriptGUISceneTreeView* thisPtr);

		ScriptGUISceneTreeView(MonoObject* instance, GUISceneTreeView* treeView);
	};
}
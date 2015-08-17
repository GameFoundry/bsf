#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for GUISceneTreeView.
	 */
	class BS_SCR_BED_EXPORT ScriptGUISceneTreeView : public TScriptGUIElement<ScriptGUISceneTreeView>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUISceneTreeView")

	private:
		ScriptGUISceneTreeView(MonoObject* instance, GUISceneTreeView* treeView);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, MonoString* style, MonoArray* guiOptions);
		static void internal_update(ScriptGUISceneTreeView* thisPtr);
	};
}
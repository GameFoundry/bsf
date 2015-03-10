#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptGUIElement.h"
#include "BsGUIPanelContainer.h"
#include "BsScriptGUIPanel.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIPanelContainer : public TScriptGUIElement<ScriptGUIPanelContainer>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "GUIPanelContainer")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* panel, MonoArray* guiOptions);
		static void internal_setPanel(ScriptGUIPanelContainer* nativeInstance, MonoObject* panel);

		ScriptGUIPanelContainer(MonoObject* instance, GUIPanelContainer* panelContainer);
	};
}
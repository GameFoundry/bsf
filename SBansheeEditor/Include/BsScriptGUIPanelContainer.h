#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsGUIPanelContainer.h"
#include "BsScriptGUIPanel.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptGUIPanelContainer : public ScriptObject<ScriptGUIPanelContainer>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "GUIPanelContainer")

	private:
		static void internal_createInstance(MonoObject* instance, MonoObject* panel, MonoArray* guiOptions);
		static void internal_setPanel(ScriptGUIPanelContainer* nativeInstance, MonoObject* panel);

		ScriptGUIPanelContainer(MonoObject* instance, GUIPanelContainer* panelContainer);

		GUIPanelContainer* mGUIPanelContainer;
	};
}
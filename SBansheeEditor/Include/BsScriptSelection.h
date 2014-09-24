#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptSelection : public ScriptObject<ScriptSelection>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "Selection")

	private:
		static void internal_GetSceneObjectSelection(MonoArray** selection);
		static void internal_SetSceneObjectSelection(MonoArray* selection);

		static void internal_GetResourceUUIDSelection(MonoArray** selection);
		static void internal_SetResourceUUIDSelection(MonoArray* selection);

		static void internal_GetResourcePathSelection(MonoArray** selection);
		static void internal_SetResourcePathSelection(MonoArray* selection);
	};
}
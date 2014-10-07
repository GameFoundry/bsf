#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptEditorUtility : public ScriptObject <ScriptEditorUtility>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "EditorUtility")

	private:
		static void internal_CalculateBounds(MonoObject* so, AABox* bounds);
		static void internal_CalculateBoundsArray(MonoArray* objects, AABox* bounds);

		ScriptEditorUtility(MonoObject* instance);
	};
}
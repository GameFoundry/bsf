#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptCodeEditor : public ScriptObject<ScriptCodeEditor>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "CodeEditor")

	private:
		static void internal_SetActiveEditor(CodeEditorType type);
		static MonoArray* internal_GetAvailableEditors();
		static void internal_OpenFile(MonoString* path, UINT32 line);
		static void internal_SyncSolution();

		ScriptCodeEditor(MonoObject* instance);
	};
}
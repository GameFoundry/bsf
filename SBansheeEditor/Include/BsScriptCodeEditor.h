#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for CodeEditorManager.
	 */
	class BS_SCR_BED_EXPORT ScriptCodeEditor : public ScriptObject<ScriptCodeEditor>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "CodeEditor")

	private:
		ScriptCodeEditor(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_SetActiveEditor(CodeEditorType type);
		static MonoArray* internal_GetAvailableEditors();
		static void internal_OpenFile(MonoString* path, UINT32 line);
		static void internal_SyncSolution();
	};
}
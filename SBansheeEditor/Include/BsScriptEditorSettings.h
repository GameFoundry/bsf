#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptEditorSettings : public ScriptObject < ScriptEditorSettings >
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "EditorSettings")

	private:
		ScriptEditorSettings(MonoObject* instance);

		static bool internal_GetMoveHandleSnapActive();
		static void internal_SetMoveHandleSnapActive(bool value);
		static bool internal_GetRotateHandleSnapActive();
		static void internal_SetRotateHandleSnapActive(bool value);
		static float internal_GetMoveHandleSnapAmount();
		static void internal_SetMoveHandleSnapAmount(float value);
		static float internal_GetRotateHandleSnapAmount();
		static void internal_SetRotateHandleSnapAmount(float value);
		static float internal_GetDefaultHandleSize();
		static void internal_SetDefaultHandleSize(float value);
		static UINT32 internal_GetActiveSceneTool();
		static void internal_SetActiveSceneTool(UINT32 value);
		static UINT32 internal_GetActiveCoordinateMode();
		static void internal_SetActiveCoordinateMode(UINT32 value);
		static UINT32 internal_GetActivePivotMode();
		static void internal_SetActivePivotMode(UINT32 value);
		static UINT32 internal_GetHash();
	};
}
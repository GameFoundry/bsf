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

		static void internal_SetFloat(MonoString* name, float value);
		static void internal_SetInt(MonoString* name, int value);
		static void internal_SetBool(MonoString* name, bool value);
		static void internal_SetString(MonoString* name, MonoString* value);

		static float internal_GetFloat(MonoString* name, float defaultValue);
		static int internal_GetInt(MonoString* name, int defaultValue);
		static bool internal_GetBool(MonoString* name, bool defaultValue);
		static MonoString* internal_GetString(MonoString* name, MonoString* defaultValue);

		static bool internal_HasKey(MonoString* name);
		static void internal_DeleteKey(MonoString* name);
		static void internal_DeleteAllKeys();

		static UINT32 internal_GetHash();
	};
}
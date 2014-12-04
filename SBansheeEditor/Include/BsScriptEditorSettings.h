#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptEditorSettings : public ScriptObject < ScriptEditorSettings >
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "EditorSettings")

	private:
		ScriptEditorSettings(MonoObject* instance);

		static bool internal_GetMoveHandleSnapActive();
		static void internal_SetMoveHandleSnapActive(bool value);
		static bool internal_GetRotateHandleSnapActive();
		static void internal_SetRotateHandleSnapActive(bool value);
		static bool internal_GetScaleHandleSnapActive();
		static void internal_SetScaleHandleSnapActive(bool value);
		static float internal_GetMoveHandleSnapAmount();
		static void internal_SetMoveHandleSnapAmount(float value);
		static float internal_GetRotateHandleSnapAmount();
		static void internal_SetRotateHandleSnapAmount(float value);
		static float internal_GetScaleHandleSnapAmount();
		static void internal_SetScaleHandleSnapAmount(float value);
		static float internal_GetDefaultHandleSize();
		static void internal_SetDefaultHandleSize(float value);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptVirtualInput : public ScriptObject<ScriptVirtualInput>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "VirtualInput")

		static void startUp();
		static void shutDown();
	private:
		static void onButtonDown(const VirtualButton& btn, UINT32 deviceIdx);
		static void onButtonUp(const VirtualButton& btn, UINT32 deviceIdx);
		static void onButtonHeld(const VirtualButton& btn, UINT32 deviceIdx);

		static MonoObject* internal_getKeyConfig();
		static void internal_setKeyConfig(MonoObject* keyConfig);
		static bool internal_isButtonHeld(VirtualButton btn, UINT32 deviceIdx);
		static bool internal_isButtonDown(VirtualButton btn, UINT32 deviceIdx);
		static bool internal_isButtonUp(VirtualButton btn, UINT32 deviceIdx);
		static float internal_getAxisValue(VirtualAxis axis, UINT32 deviceIdx);

		typedef void(__stdcall *OnButtonEventThunkDef) (VirtualButton, UINT32, MonoException**);

		static OnButtonEventThunkDef OnButtonUpThunk;
		static OnButtonEventThunkDef OnButtonDownThunk;
		static OnButtonEventThunkDef OnButtonHeldThunk;

		static HEvent OnButtonPressedConn;
		static HEvent OnButtonReleasedConn;
		static HEvent OnButtonHeldConn;

		ScriptVirtualInput(MonoObject* instance);
	};
}
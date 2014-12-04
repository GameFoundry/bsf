#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputFwd.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptInput : public ScriptObject<ScriptInput>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Input")

		static void startUp();
		static void shutDown();
	private:
		static void onButtonDown(const ButtonEvent& ev);
		static void onButtonUp(const ButtonEvent& ev);
		static void onCharInput(const TextInputEvent& ev);
		static void onPointerMoved(const PointerEvent& ev);

		static void onPointerPressed(const PointerEvent& ev);
		static void onPointerReleased(const PointerEvent& ev);
		static void onPointerDoubleClick(const PointerEvent& ev);

		static bool internal_isButtonHeld(ButtonCode code, UINT32 deviceIdx);
		static bool internal_isButtonDown(ButtonCode code, UINT32 deviceIdx);
		static bool internal_isButtonUp(ButtonCode code, UINT32 deviceIdx);
		static float internal_getAxisValue(UINT32 axisType, UINT32 deviceIdx);
		static void internal_getPointerPosition(Vector2I* position);

		typedef void(__stdcall *OnButtonEventThunkDef) (ButtonCode, UINT32, MonoException**);
		typedef void(__stdcall *OnCharInputEventThunkDef) (UINT32, MonoException**);
		typedef void(__stdcall *OnPointerEventThunkDef) (MonoObject*, PointerEventButton, 
			bool, bool, bool, float, MonoException**);

		static OnButtonEventThunkDef OnButtonPressedThunk;
		static OnButtonEventThunkDef OnButtonReleasedThunk;
		static OnCharInputEventThunkDef OnCharInputThunk;
		static OnPointerEventThunkDef OnPointerPressedThunk;
		static OnPointerEventThunkDef OnPointerReleasedThunk;
		static OnPointerEventThunkDef OnPointerMovedThunk;
		static OnPointerEventThunkDef OnPointerDoubleClickThunk;

		static HEvent OnButtonPressedConn;
		static HEvent OnButtonReleasedConn;
		static HEvent OnCharInputConn;
		static HEvent OnPointerPressedConn;
		static HEvent OnPointerReleasedConn;
		static HEvent OnPointerMovedConn;
		static HEvent OnPointerDoubleClickConn;

		ScriptInput(MonoObject* instance);
	};
}
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for VirtualInput.
	 */
	class BS_SCR_BE_EXPORT ScriptVirtualInput : public ScriptObject<ScriptVirtualInput>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "VirtualInput")

		/**
		 * @brief	Must be called on library load. Hooks up necessary callbacks.
		 */
		static void startUp();

		/**
		 * @brief	Must be called before library shutdown. Releases previously hooked callbacks.
		 */
		static void shutDown();
	private:
		/**
		 * @brief	Triggered whenever a virtual button is pressed.
		 *
		 * @param	btn			Virtual button that was pressed.
		 * @param	deviceIdx	Index of the device the button was pressed on.
		 */
		static void onButtonDown(const VirtualButton& btn, UINT32 deviceIdx);

		/**
		 * @brief	Triggered whenever a virtual button is released.
		 *
		 * @param	btn			Virtual button that was released.
		 * @param	deviceIdx	Index of the device the button was released on.
		 */
		static void onButtonUp(const VirtualButton& btn, UINT32 deviceIdx);

		/**
		 * @brief	Triggered every frame while a virtual button is held down.
		 *
		 * @param	btn			Virtual button that is being held.
		 * @param	deviceIdx	Index of the device the button is held.
		 */
		static void onButtonHeld(const VirtualButton& btn, UINT32 deviceIdx);

		static HEvent OnButtonPressedConn;
		static HEvent OnButtonReleasedConn;
		static HEvent OnButtonHeldConn;

		ScriptVirtualInput(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/

		static MonoObject* internal_getKeyConfig();
		static void internal_setKeyConfig(MonoObject* keyConfig);
		static bool internal_isButtonHeld(VirtualButton btn, UINT32 deviceIdx);
		static bool internal_isButtonDown(VirtualButton btn, UINT32 deviceIdx);
		static bool internal_isButtonUp(VirtualButton btn, UINT32 deviceIdx);
		static float internal_getAxisValue(VirtualAxis axis, UINT32 deviceIdx);

		typedef void(__stdcall *OnButtonEventThunkDef) (MonoObject*, UINT32, MonoException**);

		static OnButtonEventThunkDef OnButtonUpThunk;
		static OnButtonEventThunkDef OnButtonDownThunk;
		static OnButtonEventThunkDef OnButtonHeldThunk;
	};
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for EditorVirtualInput.
	 */
	class BS_SCR_BED_EXPORT ScriptEditorVirtualInput : public ScriptObject<ScriptEditorVirtualInput>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEngine", "EditorVirtualInput")

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

		ScriptEditorVirtualInput(MonoObject* instance);

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/

		typedef void(__stdcall *OnButtonEventThunkDef) (MonoObject*, UINT32, MonoException**);

		static OnButtonEventThunkDef OnButtonUpThunk;
		static OnButtonEventThunkDef OnButtonDownThunk;
		static OnButtonEventThunkDef OnButtonHeldThunk;
	};
}
//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsPlatform.h"
#include "BsWin32Defs.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains the main message loop
	 *
	 * @note	This is separated from the main Platform because we don't want to include various Windows
	 * 			defines in a lot of our code that includes "Platform.h".
	 */
	class BS_CORE_EXPORT PlatformWndProc : public Platform
	{
	public:
		/**
		 * @brief	Main message loop callback that processes messages received from windows.
		 */
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static bool isShiftPressed;
		static bool isCtrlPressed;

		/**
		 * @brief	Translate engine non client area to win32 non client area.
		 */
		static LRESULT translateNonClientAreaType(NonClientAreaBorderType type);

		/**
		 * @brief	Method triggerend whenever a mouse event happens.
		 */
		static void getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, Vector2I& mousePos, OSPointerButtonStates& btnStates);

		/**
		 * @brief	Converts a virtual key code into an input command, if possible. Returns true
		 *			if conversion was done.
		 */
		static bool getCommand(unsigned int virtualKeyCode, InputCommandType& command);
	};
}
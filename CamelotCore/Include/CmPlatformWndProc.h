#pragma once

#include "CmPlatform.h"
#include "CmWin32Defs.h"

namespace CamelotFramework
{
	/**
	 * @note	This is separated from the main Platform because we don't want to include various Windows
	 * 			defines in a lot of our code that includes "Platform.h"
	 */
	class CM_EXPORT PlatformWndProc : public Platform
	{
	public:
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static bool isShiftPressed;
		static bool isCtrlPressed;

		static LRESULT translateNonClientAreaType(NonClientAreaBorderType type);

		static void getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, Vector2I& mousePos, OSPositionalInputButtonStates& btnStates);
		static bool getCommand(unsigned int virtualKeyCode, InputCommandType& command);
	};
}
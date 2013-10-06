#pragma once

#include "CmPlatform.h"
#include "CmWin32Defs.h"

namespace CamelotFramework
{
	class CM_EXPORT PlatformWndProc : public Platform
	{
	public:
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		static UINT32 mMoveResizeMouseUpState; // 0 - Didn't receive and not waiting for mouse up, 1 - Awaiting mouse up, 2 - Received mouse up

		static LRESULT translateNonClientAreaType(NonClientAreaBorderType type);

		static void getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, Int2& mousePos, OSPositionalInputButtonStates& btnStates);
	};
}
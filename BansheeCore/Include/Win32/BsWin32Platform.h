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
	class BS_CORE_EXPORT Win32Platform : public Platform
	{
	public:
		/**
		 * @brief	Main message loop callback that processes messages received from windows.
		 */
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
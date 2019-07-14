//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Platform/BsPlatform.h"

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif
#include <windows.h>
#include <windowsx.h>

#define WM_BS_SETCAPTURE WM_USER + 101
#define WM_BS_RELEASECAPTURE WM_USER + 102

namespace bs
{
	/** @addtogroup Platform-Internal
	 *  @{
	 */

	/** Various Win32 specific functionality. Contains the main message loop. */
	class BS_CORE_EXPORT Win32Platform : public Platform
	{
	public:
		/** Called when a new DropTarget gets created. */
		static void registerDropTarget(DropTarget* target);

		/** Called just before a DropTarget gets destroyed. */
		static void unregisterDropTarget(DropTarget* target);

		/** Main message loop callback that processes messages received from windows. */
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};

	/** @} */
}

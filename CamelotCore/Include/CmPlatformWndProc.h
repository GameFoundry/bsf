#pragma once

#include "CmPlatform.h"

#define WM_CM_HIDECURSOR WM_USER + 101
#define WM_CM_SHOWCURSOR WM_USER + 102

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // Required to stop windows.h messing up std::min
#endif
#include <windows.h>
#include <windowsx.h>

namespace CamelotFramework
{
	class CM_EXPORT PlatformWndProc : public Platform
	{
	public:
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
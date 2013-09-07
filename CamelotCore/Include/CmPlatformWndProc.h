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
		static LRESULT translateNonClientAreaType(NonClientAreaBorderType type);
	};
}
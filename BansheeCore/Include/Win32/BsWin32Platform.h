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
		 * @brief	Creates a new bitmap usable by various Win32 methods from the provided pixel data.
		 *			Caller must ensure to call ::DeleteObject on the bitmap handle when finished.
		 */
		static HBITMAP createBitmap(const PixelDataPtr& pixelData, bool premultiplyAlpha);

		/**
		 * @brief	Main message loop callback that processes messages received from windows.
		 */
		static LRESULT CALLBACK _win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
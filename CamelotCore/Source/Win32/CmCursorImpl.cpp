#include "Win32/CmCursorImpl.h"
#include "CmRenderWindow.h"

namespace CamelotFramework
{
	Int2 Cursor::getWindowPosition(RenderWindow& window)
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		HWND hwnd;
		window.getCustomAttribute("WINDOW", &hwnd);

		ScreenToClient(hwnd, &screenPos);

		return Int2(screenPos.x, screenPos.y);
	}

	void Cursor::setWindowPosition(RenderWindow& window, const Int2& pos)
	{
		POINT screenPos;

		// Convert client coordinates to screen coordinates
		screenPos.x = pos.x;
		screenPos.y = pos.y;

		HWND hwnd;
		window.getCustomAttribute("WINDOW", &hwnd);

		ClientToScreen(hwnd, &screenPos);

		SetCursorPos(screenPos.x, screenPos.y);
	}

	static Int2 getScreenPosition()
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		return Int2(screenPos.x, screenPos.y);
	}

	static void setScreenPosition(const Int2& pos)
	{
		POINT screenPos;

		// Convert client coordinates to screen coordinates
		screenPos.x = pos.x;
		screenPos.y = pos.y;

		SetCursorPos(screenPos.x, screenPos.y);
	}

	void Cursor::hide()
	{
		RECT clipWindowRect;

		ShowCursor(FALSE);
		SetCursor(0);

		//RenderWindowPtr primaryWindow = gApplication().getPrimaryRenderWindow();
		//HWND hwnd;
		//primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		//// Clip cursor to the window
		//if( GetWindowRect(hwnd, &clipWindowRect))
		//{
		//	ClipCursor(&clipWindowRect);
		//}

		//// Capture cursor to user window
		//SetCapture(hwnd);
	}

	void Cursor::show()
	{
		//// Un-capture cursor
		//ReleaseCapture();

		//// Release the cursor from the window
		//ClipCursor(NULL);

		//ShowCursor(TRUE);
	}
}
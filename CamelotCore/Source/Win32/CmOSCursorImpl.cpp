#include "Win32/CmOSCursorImpl.h"
#include "CmApplication.h"
#include "CmRenderWindow.h"

namespace CamelotFramework
{
	Int2 OSCursor::getPosition()
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		RenderWindowPtr primaryWindow = gApplication().getPrimaryRenderWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		ScreenToClient(hwnd, &screenPos);

		return Int2(screenPos.x, screenPos.y);
	}

	void OSCursor::setPosition(const Int2& pos)
	{
		POINT screenPos;

		// Convert client coordinates to screen coordinates
		screenPos.x = pos.x;
		screenPos.y = pos.y;

		RenderWindowPtr primaryWindow = gApplication().getPrimaryRenderWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		ClientToScreen(hwnd, &screenPos);

		SetCursorPos(screenPos.x, screenPos.y);
	}

	void OSCursor::hide()
	{
		RECT clipWindowRect;

		ShowCursor( FALSE );

		RenderWindowPtr primaryWindow = gApplication().getPrimaryRenderWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		// Clip cursor to the window
		if( GetWindowRect(hwnd, &clipWindowRect))
		{
			ClipCursor(&clipWindowRect);
		}

		// Capture cursor to user window
		SetCapture(hwnd);
	}

	void OSCursor::show()
	{
		// Un-capture cursor
		ReleaseCapture();

		// Release the cursor from the window
		ClipCursor(NULL);

		ShowCursor(TRUE);
	}
}
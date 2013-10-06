#include "CmPlatformWndProc.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmInput.h"

namespace CamelotFramework
{
	UINT32 PlatformWndProc::mMoveResizeMouseUpState = 0;

	LRESULT CALLBACK PlatformWndProc::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));
			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		RenderWindow* win = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		switch( uMsg )
		{
		case WM_ACTIVATE:
			{
				bool active = (LOWORD(wParam) != WA_INACTIVE);
				if( active )
				{
					win->setActive(true);

					if(!win->hasFocus())
						windowFocusReceived(win);
				}
				else
				{
					if(win->hasFocus())
						windowFocusLost(win);
				}

				break;
			}
		case WM_SYSKEYDOWN:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSKEYUP:
			switch( wParam )
			{
			case VK_CONTROL:
			case VK_SHIFT:
			case VK_MENU: //ALT
			case VK_F10:
				//return zero to bypass defProc and signal we processed the message
				return 0;
			}
			break;
		case WM_SYSCHAR:
			// return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
			if (wParam != VK_SPACE)
				return 0;
			break;
		case WM_MOVE:
			windowMovedOrResized(win);
			break;
		case WM_DISPLAYCHANGE:
			windowMovedOrResized(win);
			break;
		case WM_SIZE:
			windowMovedOrResized(win);
			break;
		case WM_SETCURSOR:
			if(isCursorHidden())
				win32HideCursor();
			else
			{
				switch (LOWORD(lParam))
				{
				case HTTOPLEFT:
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
					return 0;
				case HTTOP:
					SetCursor(LoadCursor(0, IDC_SIZENS));
					return 0;
				case HTTOPRIGHT:
					SetCursor(LoadCursor(0, IDC_SIZENESW));
					return 0;
				case HTLEFT:
					SetCursor(LoadCursor(0, IDC_SIZEWE));
					return 0;
				case HTRIGHT:
					SetCursor(LoadCursor(0, IDC_SIZEWE));
					return 0;
				case HTBOTTOMLEFT:
					SetCursor(LoadCursor(0, IDC_SIZENESW));
					return 0;
				case HTBOTTOM:
					SetCursor(LoadCursor(0, IDC_SIZENS));
					return 0;
				case HTBOTTOMRIGHT:
					SetCursor(LoadCursor(0, IDC_SIZENWSE));
					return 0;
				}

				win32ShowCursor();
			}
			return true;
		case WM_GETMINMAXINFO:
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;
			break;
		case WM_CLOSE:
			{
				// TODO - Only stop main loop if primary window is closed!!
				gApplication().stopMainLoop();

				return 0;
			}
		case WM_NCHITTEST:
			{
				auto iterFind = mNonClientAreas.find(win);
				if(iterFind == mNonClientAreas.end())
					break;

				POINT mousePos;
				mousePos.x = GET_X_LPARAM(lParam);
				mousePos.y = GET_Y_LPARAM(lParam); 

				ScreenToClient(hWnd, &mousePos);

				Int2 mousePosInt;
				mousePosInt.x = mousePos.x;
				mousePosInt.y = mousePos.y;

				Vector<NonClientResizeArea>::type& resizeAreasPerWindow = iterFind->second.resizeAreas;
				for(auto area : resizeAreasPerWindow)
				{
					if(area.area.contains(mousePosInt))
						return translateNonClientAreaType(area.type);
				}

				Vector<Rect>::type& moveAreasPerWindow = iterFind->second.moveAreas;
				for(auto area : moveAreasPerWindow)
				{
					if(area.contains(mousePosInt))
						return HTCAPTION;
				}

				return HTCLIENT;
			}
		case WM_MOUSELEAVE:
			{
				// Note: Right now I track only mouse leaving client area. So it's possible for the "mouse left window" callback
				// to trigger, while the mouse is still in the non-client area of the window.
				mIsTrackingMouse = false; // TrackMouseEvent ends when this message is received and needs to be re-applied

				CM_LOCK_MUTEX(mSync);

				mMouseLeftWindows.push_back(win);
			}
			break;
		case WM_LBUTTONUP:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Left, btnStates);
			}
			break;
		case WM_MBUTTONUP:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Middle, btnStates);
			}
			break;
		case WM_RBUTTONUP:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Right, btnStates);
			}
			break;
		case WM_LBUTTONDOWN:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Left, btnStates);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Middle, btnStates);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Right, btnStates);
			}
			break;
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
			{
				// Set up tracking so we get notified when mouse leaves the window
				if(!mIsTrackingMouse)
				{
					TRACKMOUSEEVENT tme = { sizeof(tme) };
					tme.dwFlags = TME_LEAVE;

					tme.hwndTrack = hWnd;
					TrackMouseEvent(&tme);

					mIsTrackingMouse = true;
				}

				if(uMsg == WM_NCMOUSEMOVE)
					return true;

				Int2 intMousePos;
				OSPositionalInputButtonStates btnStates;
				
				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorMoved.empty())
					onCursorMoved(intMousePos, btnStates);

				return true;
			}
		case WM_MOUSEWHEEL:
			{
				INT16 wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);

				float wheelDeltaFlt = wheelDelta / (float)WHEEL_DELTA;
				if(!onMouseWheelScrolled.empty())
					onMouseWheelScrolled(wheelDeltaFlt);

				return true;
			}
		case WM_DEADCHAR:
		case WM_CHAR:
			{
				switch (wParam) 
				{ 
				case VK_BACK:
				case 0x0A:  // linefeed 
				case 0x0D:  // carriage return 
				case VK_ESCAPE:
				case VK_TAB: 
					break; 

				default:    // displayable character 
					{
						UINT8 scanCode = (lParam >> 16) & 0xFF;

						BYTE keyState[256];
						HKL layout = GetKeyboardLayout(0);
						if(GetKeyboardState(keyState) == 0)
							return 0;

						unsigned int vk = MapVirtualKeyEx(scanCode, MAPVK_VSC_TO_VK_EX, layout);
						if(vk == 0)
							return 0;

						bool isDeadKey = (MapVirtualKeyEx(vk, MAPVK_VK_TO_CHAR, layout) & (1 << 31)) != 0;
						if(isDeadKey)
							return 0;

						wchar_t buff[3] = {0};
						int numChars = ToUnicodeEx(vk, scanCode, keyState, buff, 3, 0, layout);

						// TODO - I am ignoring dead keys here - primarily because I haven't found a good way of retrieving non-combined dead key
						// value. ToUnicodeEx and MapVirtualKeyEx only return precombined (i.e. spacing) versions, which can't be combined using other characters.
						// I need non-combined version so I can use it with FoldString to apply to a certain character.

						UINT32 finalChar = 0;
						if(numChars == 1)
							finalChar = buff[0];
						else
							return 0;

						if(!onCharInput.empty())
							onCharInput(finalChar);

						return 0;
					}
				} 

				break;
			}
		case WM_CM_SETCAPTURE:
			SetCapture(hWnd);
			break;
		case WM_CM_RELEASECAPTURE:
			ReleaseCapture();
			break;
		case WM_CAPTURECHANGED:
			if(!onMouseCaptureChanged.empty())
				onMouseCaptureChanged();
			break;
		}

		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}

	LRESULT PlatformWndProc::translateNonClientAreaType(NonClientAreaBorderType type)
	{
		LRESULT dir = HTCLIENT;
		switch(type)
		{
		case NonClientAreaBorderType::Left:
			dir = HTLEFT;
			break;
		case NonClientAreaBorderType::TopLeft:
			dir = HTTOPLEFT;
			break;
		case NonClientAreaBorderType::Top:
			dir = HTTOP;
			break;
		case NonClientAreaBorderType::TopRight:
			dir = HTTOPRIGHT;
			break;
		case NonClientAreaBorderType::Right:
			dir = HTRIGHT;
			break;
		case NonClientAreaBorderType::BottomRight:
			dir = HTBOTTOMRIGHT;
			break;
		case NonClientAreaBorderType::Bottom:
			dir = HTBOTTOM;
			break;
		case NonClientAreaBorderType::BottomLeft:
			dir = HTBOTTOMLEFT;
			break;
		}

		return dir;
	}

	void PlatformWndProc::getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, Int2& mousePos, OSPositionalInputButtonStates& btnStates)
	{
		POINT clientPoint;

		clientPoint.x = GET_X_LPARAM(lParam);
		clientPoint.y = GET_Y_LPARAM(lParam); 

		ClientToScreen(hWnd, &clientPoint);

		mousePos.x = clientPoint.x;
		mousePos.y = clientPoint.y;

		btnStates.mouseButtons[0] = (wParam & MK_LBUTTON) != 0;
		btnStates.mouseButtons[1] = (wParam & MK_MBUTTON) != 0;
		btnStates.mouseButtons[2] = (wParam & MK_RBUTTON) != 0;
		btnStates.shift = (wParam & MK_SHIFT) != 0;
		btnStates.ctrl = (wParam & MK_CONTROL) != 0;
	}
}
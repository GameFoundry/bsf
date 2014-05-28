#include "Win32/CmPlatformWndProc.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"
#include "CmInput.h"
#include "CmDebug.h"
#include "CmRenderWindowManager.h"

namespace BansheeEngine
{
	bool PlatformWndProc::isShiftPressed = false;
	bool PlatformWndProc::isCtrlPressed = false;

	LRESULT CALLBACK PlatformWndProc::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));

			RenderWindow* newWindow = (RenderWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if(newWindow->isModal())
			{
				if(!mModalWindowStack.empty())
				{
					RenderWindow* curModalWindow = mModalWindowStack.top();

					HWND curHwnd;
					curModalWindow->getCustomAttribute("WINDOW", &curHwnd);
					EnableWindow(curHwnd, FALSE);
				}
				else
				{
					Vector<RenderWindow*> renderWindows = RenderWindowManager::instance().getRenderWindows();
					for(auto& renderWindow : renderWindows)
					{
						if(renderWindow == newWindow)
							continue;

						HWND curHwnd;
						renderWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow(curHwnd, FALSE);
					}
				}

				mModalWindowStack.push(newWindow);
			}

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
					win->setActive(true);

				break;
			}
		case WM_DESTROY:
			{
				bool reenableWindows = false;
				if(!mModalWindowStack.empty())
				{
					if(mModalWindowStack.top() == win) // This is the most common case, top-most modal was closed
					{
						mModalWindowStack.pop();
					}
					else // Possibly some other window was closed somehow, see if it was modal and remove from stack if it is
					{
						Stack<RenderWindow*> newStack;

						while(!mModalWindowStack.empty())
						{
							RenderWindow* curWindow = mModalWindowStack.top();
							mModalWindowStack.pop();

							if(curWindow == win)
								continue;

							newStack.push(curWindow);
						}

						mModalWindowStack = newStack;
					}

					if(!mModalWindowStack.empty()) // Enable next modal window
					{
						RenderWindow* curModalWindow = mModalWindowStack.top();

						HWND curHwnd;
						curModalWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow(curHwnd, TRUE);
					}
					else
						reenableWindows = true; // No more modal windows, re-enable any remaining window
				}

				if(reenableWindows)
				{
					Vector<RenderWindow*> renderWindows = RenderWindowManager::instance().getRenderWindows();
					for(auto& renderWindow : renderWindows)
					{
						HWND curHwnd;
						renderWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow(curHwnd, TRUE);
					}
				}

				break;
			}
		case WM_SETFOCUS:
			{
				if(!win->hasFocus())
					windowFocusReceived(win);

				break;
			}
		case WM_KILLFOCUS:
			{
				if(win->hasFocus())
					windowFocusLost(win);

				break;
			}
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

				Vector2I mousePosInt;
				mousePosInt.x = mousePos.x;
				mousePosInt.y = mousePos.y;

				Vector<NonClientResizeArea>& resizeAreasPerWindow = iterFind->second.resizeAreas;
				for(auto area : resizeAreasPerWindow)
				{
					if(area.area.contains(mousePosInt))
						return translateNonClientAreaType(area.type);
				}

				Vector<RectI>& moveAreasPerWindow = iterFind->second.moveAreas;
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
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Left, btnStates);
			}
			break;
		case WM_MBUTTONUP:
			{
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Middle, btnStates);
			}
			break;
		case WM_RBUTTONUP:
			{
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Right, btnStates);
			}
			break;
		case WM_LBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Left, btnStates);
			}
			break;
		case WM_MBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Middle, btnStates);
			}
			break;
		case WM_RBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Right, btnStates);
			}
			break;
		case WM_LBUTTONDBLCLK:
			{
				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, intMousePos, btnStates);

				if(!onCursorDoubleClick.empty())
					onCursorDoubleClick(intMousePos, btnStates);
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

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;
				
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
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
			{
				if(wParam == VK_SHIFT)
				{
					isShiftPressed = true;
					break;
				}

				if(wParam == VK_CONTROL)
				{
					isCtrlPressed = true;
					break;
				}

				InputCommandType command = InputCommandType::Backspace;
				if(getCommand((unsigned int)wParam, command))
				{
					if(!onInputCommand.empty())
						onInputCommand(command);

					return 0;
				}

				break;
			}
		case WM_SYSKEYUP:
		case WM_KEYUP:
			{
				if(wParam == VK_SHIFT)
				{
					isShiftPressed = false;
				}

				if(wParam == VK_CONTROL)
				{
					isCtrlPressed = false;
				}

				break;
			}
		case WM_CHAR:
			{
				// TODO - Not handling IME input

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

						InputCommandType command = InputCommandType::Backspace;
						if(getCommand(vk, command)) // We ignore character combinations that are special commands
							return 0;

						UINT32 finalChar = (UINT32)wParam;

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

	void PlatformWndProc::getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, Vector2I& mousePos, OSPointerButtonStates& btnStates)
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

	bool PlatformWndProc::getCommand(unsigned int virtualKeyCode, InputCommandType& command)
	{
		switch (virtualKeyCode) 
		{ 
		case VK_LEFT:
			command = isShiftPressed ? InputCommandType::SelectLeft : InputCommandType::CursorMoveLeft;
			return true;
		case VK_RIGHT:
			command = isShiftPressed ? InputCommandType::SelectRight : InputCommandType::CursorMoveRight;
			return true;
		case VK_UP:
			command = isShiftPressed ? InputCommandType::SelectUp : InputCommandType::CursorMoveUp;
			return true;
		case VK_DOWN:
			command = isShiftPressed ? InputCommandType::SelectDown : InputCommandType::CursorMoveDown;
			return true;
		case VK_ESCAPE:
			command = InputCommandType::Escape;
			return true;
		case VK_RETURN:
			command = InputCommandType::Return;
			return true;
		case VK_BACK:
			command = InputCommandType::Backspace;
			return true;
		case VK_DELETE:
			command = InputCommandType::Delete;
			return true;
		}

		return false;
	}
}
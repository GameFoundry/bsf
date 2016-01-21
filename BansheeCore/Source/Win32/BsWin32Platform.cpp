//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Win32/BsWin32Platform.h"
#include "BsRenderWindow.h"
#include "BsPixelUtil.h"
#include "BsCoreApplication.h"
#include "BsDebug.h"
#include "BsRenderWindowManager.h"
#include "Win32/BsWin32Defs.h"
#include "Win32/BsWin32DropTarget.h"
#include "Win32/BsWin32PlatformData.h"
#include "TimeAPI.h"

namespace BansheeEngine
{
	Event<void(const Vector2I&, OSPointerButtonStates)> Platform::onCursorMoved;
	Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> Platform::onCursorButtonPressed;
	Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> Platform::onCursorButtonReleased;
	Event<void(const Vector2I&, OSPointerButtonStates)> Platform::onCursorDoubleClick;
	Event<void(InputCommandType)> Platform::onInputCommand;
	Event<void(float)> Platform::onMouseWheelScrolled;
	Event<void(UINT32)> Platform::onCharInput;

	Event<void(RenderWindowCore*)> Platform::onMouseLeftWindow;
	Event<void()> Platform::onMouseCaptureChanged;

	Platform::Pimpl* Platform::mData = bs_new<Platform::Pimpl>();

	Platform::~Platform()
	{
		bs_delete(mData);
		mData = nullptr;
	}

	Vector2I Platform::getCursorPosition()
	{
		Vector2I screenPos;

		POINT cursorPos;
		GetCursorPos(&cursorPos);

		screenPos.x = cursorPos.x;
		screenPos.y = cursorPos.y;

		return screenPos;
	}

	void Platform::setCursorPosition(const Vector2I& screenPos)
	{
		SetCursorPos(screenPos.x, screenPos.y);
	}

	void Platform::captureMouse(const RenderWindow& window)
	{
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);
		
		PostMessage((HWND)hwnd, WM_BS_SETCAPTURE, WPARAM((HWND)hwnd), 0);
	}

	void Platform::releaseMouseCapture()
	{
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage((HWND)hwnd, WM_BS_RELEASECAPTURE, WPARAM((HWND)hwnd), 0);
	}

	bool Platform::isPointOverWindow(const RenderWindow& window, const Vector2I& screenPos)
	{
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();

		POINT point;
		point.x = screenPos.x;
		point.y = screenPos.y;

		UINT64 hwndToCheck;
		window.getCustomAttribute("WINDOW", &hwndToCheck);

		HWND hwndUnderPos = WindowFromPoint(point);
		return hwndUnderPos == (HWND)hwndToCheck;
	}

	void Platform::hideCursor()
	{
		mData->mIsCursorHidden = true;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::showCursor()
	{
		mData->mIsCursorHidden = false;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	bool Platform::isCursorHidden() 
	{ 
		return mData->mIsCursorHidden; 
	}

	void Platform::clipCursorToWindow(const RenderWindow& window)
	{
		UINT64 hwnd;
		window.getCustomAttribute("WINDOW", &hwnd);

		// Clip cursor to the window
		RECT clipWindowRect;
		if(GetWindowRect((HWND)hwnd, &clipWindowRect))
		{
			ClipCursor(&clipWindowRect);
		}
	}

	void Platform::clipCursorToRect(const Rect2I& screenRect)
	{
		RECT clipWindowRect;
		clipWindowRect.left = screenRect.x;
		clipWindowRect.top = screenRect.y;
		clipWindowRect.right = screenRect.x + screenRect.width;
		clipWindowRect.bottom = screenRect.y + screenRect.height;

		ClipCursor(&clipWindowRect);
	}

	void Platform::clipCursorDisable()
	{
		ClipCursor(NULL);
	}

	// TODO - Add support for animated custom cursor
	void Platform::setCursor(PixelData& pixelData, const Vector2I& hotSpot)
	{
		if (mData->mUsingCustomCursor)
		{
			SetCursor(0);
			DestroyIcon(mData->mCursor.cursor);
		}

		mData->mUsingCustomCursor = true;

		HBITMAP hBitmap = Win32Platform::createBitmap(pixelData, false); 
		HBITMAP hMonoBitmap = CreateBitmap(pixelData.getWidth(), pixelData.getHeight(), 1, 1, nullptr);

		ICONINFO iconinfo = {0};
		iconinfo.fIcon = FALSE;
		iconinfo.xHotspot = (DWORD)hotSpot.x;
		iconinfo.yHotspot = (DWORD)hotSpot.y;
		iconinfo.hbmMask = hMonoBitmap;
		iconinfo.hbmColor = hBitmap;

		mData->mCursor.cursor = CreateIconIndirect(&iconinfo);
		
		DeleteObject(hBitmap);          
		DeleteObject(hMonoBitmap);

		// Make sure we notify the message loop to perform the actual cursor update
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage((HWND)hwnd, WM_SETCURSOR, WPARAM((HWND)hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::setIcon(const PixelData& pixelData)
	{
		PixelDataPtr resizedData = PixelData::create(32, 32, 1, PF_R8G8B8A8);
		PixelUtil::scale(pixelData, *resizedData);

		HBITMAP hBitmap = Win32Platform::createBitmap(pixelData, false);
		HBITMAP hMonoBitmap = CreateBitmap(pixelData.getWidth(), pixelData.getHeight(), 1, 1, nullptr);

		ICONINFO iconinfo = { 0 };
		iconinfo.fIcon = TRUE;
		iconinfo.xHotspot = 0;
		iconinfo.yHotspot = 0;
		iconinfo.hbmMask = hMonoBitmap;
		iconinfo.hbmColor = hBitmap;

		HICON icon = CreateIconIndirect(&iconinfo);

		DeleteObject(hBitmap);
		DeleteObject(hMonoBitmap);

		// Make sure we notify the message loop to perform the actual cursor update
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		UINT64 hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);
		
		PostMessage((HWND)hwnd, WM_SETICON, WPARAM(ICON_BIG), (LPARAM)icon);
	}

	void Platform::setCaptionNonClientAreas(const RenderWindowCore& window, const Vector<Rect2I>& nonClientAreas)
	{
		BS_LOCK_MUTEX(mData->mSync);

		mData->mNonClientAreas[&window].moveAreas = nonClientAreas;
	}

	void Platform::setResizeNonClientAreas(const RenderWindowCore& window, const Vector<NonClientResizeArea>& nonClientAreas)
	{
		BS_LOCK_MUTEX(mData->mSync);

		mData->mNonClientAreas[&window].resizeAreas = nonClientAreas;
	}

	void Platform::resetNonClientAreas(const RenderWindowCore& window)
	{
		BS_LOCK_MUTEX(mData->mSync);

		auto iterFind = mData->mNonClientAreas.find(&window);

		if (iterFind != end(mData->mNonClientAreas))
			mData->mNonClientAreas.erase(iterFind);
	}

	void Platform::sleep(UINT32 duration)
	{
		Sleep((DWORD)duration);
	}

	OSDropTarget& Platform::createDropTarget(const RenderWindow* window, int x, int y, unsigned int width, unsigned int height)
	{
		Win32DropTarget* win32DropTarget = nullptr;
		auto iterFind = mData->mDropTargets.dropTargetsPerWindow.find(window);
		if (iterFind == mData->mDropTargets.dropTargetsPerWindow.end())
		{
			UINT64 hwnd;
			window->getCustomAttribute("WINDOW", &hwnd);

			win32DropTarget = bs_new<Win32DropTarget>((HWND)hwnd);
			mData->mDropTargets.dropTargetsPerWindow[window] = win32DropTarget;

			{
				BS_LOCK_MUTEX(mData->mSync);
				mData->mDropTargets.dropTargetsToInitialize.push_back(win32DropTarget);
			}
		}
		else
			win32DropTarget = iterFind->second;

		OSDropTarget* newDropTarget = new (bs_alloc<OSDropTarget>()) OSDropTarget(window, x, y, width, height);
		win32DropTarget->registerDropTarget(newDropTarget);

		return *newDropTarget;
	}

	void Platform::destroyDropTarget(OSDropTarget& target)
	{
		auto iterFind = mData->mDropTargets.dropTargetsPerWindow.find(target.getOwnerWindow());
		if (iterFind == mData->mDropTargets.dropTargetsPerWindow.end())
		{
			LOGWRN("Attempting to destroy a drop target but cannot find its parent window.");
		}
		else
		{
			Win32DropTarget* win32DropTarget = iterFind->second;
			win32DropTarget->unregisterDropTarget(&target);

			if(win32DropTarget->getNumDropTargets() == 0)
			{
				mData->mDropTargets.dropTargetsPerWindow.erase(iterFind);

				{
					BS_LOCK_MUTEX(mData->mSync);
					mData->mDropTargets.dropTargetsToDestroy.push_back(win32DropTarget);
				}
			}
		}
		
		BS_PVT_DELETE(OSDropTarget, &target);
	}

	void Platform::_messagePump()
	{
		MSG  msg;
		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Platform::_startUp()
	{
		BS_LOCK_MUTEX(mData->mSync);

		if (timeBeginPeriod(1) == TIMERR_NOCANDO)
		{
			LOGWRN("Unable to set timer resolution to 1ms. This can cause significant waste " \
				"in performance for waiting threads.");
		}


		mData->mRequiresStartUp = true;
	}

	void Platform::_update()
	{
		for (auto& dropTarget : mData->mDropTargets.dropTargetsPerWindow)
		{
			dropTarget.second->update();
		}
	}

	void Platform::_coreUpdate()
	{
		{
			BS_LOCK_MUTEX(mData->mSync);
			if (mData->mRequiresStartUp)
			{
				OleInitialize(nullptr);

				mData->mRequiresStartUp = false;
			}
		}

		{
			BS_LOCK_MUTEX(mData->mSync);
			for (auto& dropTargetToDestroy : mData->mDropTargets.dropTargetsToDestroy)
			{
				dropTargetToDestroy->unregisterWithOS();
				dropTargetToDestroy->Release();
			}

			mData->mDropTargets.dropTargetsToDestroy.clear();
		}

		{
			BS_LOCK_MUTEX(mData->mSync);
			for (auto& dropTargetToInit : mData->mDropTargets.dropTargetsToInitialize)
			{
				dropTargetToInit->registerWithOS();
			}

			mData->mDropTargets.dropTargetsToInitialize.clear();
		}

		_messagePump();

		{
			BS_LOCK_MUTEX(mData->mSync);
			if (mData->mRequiresShutDown)
			{
				OleUninitialize();
				mData->mRequiresShutDown = false;
			}
		}
	}

	void Platform::_shutDown()
	{
		BS_LOCK_MUTEX(mData->mSync);

		timeEndPeriod(1);
		mData->mRequiresShutDown = true;
	}

	bool isShiftPressed = false;
	bool isCtrlPressed = false;

	/**
	 * @brief	Translate engine non client area to win32 non client area.
	 */
	LRESULT translateNonClientAreaType(NonClientAreaBorderType type)
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

	/**
	 * @brief	Method triggered whenever a mouse event happens.
	 */
	void getMouseData(HWND hWnd, WPARAM wParam, LPARAM lParam, bool nonClient, Vector2I& mousePos, OSPointerButtonStates& btnStates)
	{
		POINT clientPoint;

		clientPoint.x = GET_X_LPARAM(lParam);
		clientPoint.y = GET_Y_LPARAM(lParam); 

		if (!nonClient)
			ClientToScreen(hWnd, &clientPoint);

		mousePos.x = clientPoint.x;
		mousePos.y = clientPoint.y;

		btnStates.mouseButtons[0] = (wParam & MK_LBUTTON) != 0;
		btnStates.mouseButtons[1] = (wParam & MK_MBUTTON) != 0;
		btnStates.mouseButtons[2] = (wParam & MK_RBUTTON) != 0;
		btnStates.shift = (wParam & MK_SHIFT) != 0;
		btnStates.ctrl = (wParam & MK_CONTROL) != 0;
	}

	/**
	 * @brief	Converts a virtual key code into an input command, if possible. Returns true
	 *			if conversion was done.
	 */
	bool getCommand(unsigned int virtualKeyCode, InputCommandType& command)
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
			command = isShiftPressed ? InputCommandType::Return : InputCommandType::Confirm;
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

	HBITMAP Win32Platform::createBitmap(const PixelData& pixelData, bool premultiplyAlpha)
	{
		BITMAPINFO bi;

		ZeroMemory(&bi, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = pixelData.getWidth();
		bi.bmiHeader.biHeight = pixelData.getHeight();
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 32;
		bi.bmiHeader.biCompression = BI_RGB;

		HDC hDC = GetDC(nullptr);

		void* data = nullptr;
		HBITMAP hBitmap = CreateDIBSection(hDC, &bi, DIB_RGB_COLORS, (void**)&data, nullptr, 0);

		HDC hBitmapDC = CreateCompatibleDC(hDC);
		ReleaseDC(nullptr, hDC);

		//Select the bitmaps to DC
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(hBitmapDC, hBitmap);

		//Scan each pixel of the source bitmap and create the masks
		Color pixel;
		DWORD *dst = (DWORD*)data;
		for (UINT32 y = 0; y < pixelData.getHeight(); ++y)
		{
			for (UINT32 x = 0; x < pixelData.getWidth(); ++x)
			{
				pixel = pixelData.getColorAt(x, pixelData.getHeight() - y - 1);

				if (premultiplyAlpha)
				{
					pixel.r *= pixel.a;
					pixel.g *= pixel.a;
					pixel.b *= pixel.a;
				}

				*dst = pixel.getAsBGRA();

				dst++;
			}
		}

		SelectObject(hBitmapDC, hOldBitmap);
		DeleteDC(hBitmapDC);

		return hBitmap;
	}

	LRESULT CALLBACK Win32Platform::_win32WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (uMsg == WM_CREATE)
		{	// Store pointer to Win32Window in user data area
			SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)(((LPCREATESTRUCT)lParam)->lpCreateParams));

			RenderWindowCore* newWindow = (RenderWindowCore*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
			if (newWindow != nullptr)
			{
				const RenderWindowProperties& props = newWindow->getProperties();
				if (!props.isHidden())
					ShowWindow(hWnd, SW_SHOWNOACTIVATE);

				if (props.isModal())
				{
					if (!mData->mModalWindowStack.empty())
					{
						RenderWindowCore* curModalWindow = mData->mModalWindowStack.back();

						UINT64 curHwnd;
						curModalWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow((HWND)curHwnd, FALSE);
					}
					else
					{
						Vector<RenderWindowCore*> renderWindows = RenderWindowCoreManager::instance().getRenderWindows();
						for (auto& renderWindow : renderWindows)
						{
							if (renderWindow == newWindow)
								continue;

							UINT64 curHwnd;
							renderWindow->getCustomAttribute("WINDOW", &curHwnd);
							EnableWindow((HWND)curHwnd, FALSE);
						}
					}

					mData->mModalWindowStack.push_back(newWindow);
				}
				else
				{
					// A non-modal window was opened while another modal one is open:
					// immediately deactivate it and make sure the modal windows stay on top
					if (!mData->mModalWindowStack.empty())
					{
						EnableWindow((HWND)hWnd, FALSE);

						for (auto window : mData->mModalWindowStack)
						{
							UINT64 curHwnd;
							window->getCustomAttribute("WINDOW", &curHwnd);

							BringWindowToTop((HWND)curHwnd);
						}
					}
				}
			}
			else
				ShowWindow(hWnd, SW_SHOWNOACTIVATE);

			return 0;
		}

		// look up window instance
		// note: it is possible to get a WM_SIZE before WM_CREATE
		RenderWindowCore* win = (RenderWindowCore*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
		if (!win)
			return DefWindowProc(hWnd, uMsg, wParam, lParam);

		switch( uMsg )
		{
		case WM_DESTROY:
			{
				bool reenableWindows = false;
				if (!mData->mModalWindowStack.empty())
				{
					// Start from back because the most common case is closing the top-most modal window
					for (auto iter = mData->mModalWindowStack.rbegin(); iter != mData->mModalWindowStack.rend(); ++iter)
					{
						if (*iter == win)
						{
							auto iterFwd = std::next(iter).base(); // erase doesn't accept reverse iter, so convert

							mData->mModalWindowStack.erase(iterFwd);
							break;
						}
					}

					if (!mData->mModalWindowStack.empty()) // Enable next modal window
					{
						RenderWindowCore* curModalWindow = mData->mModalWindowStack.back();

						UINT64 curHwnd;
						curModalWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow((HWND)curHwnd, TRUE);
					}
					else
						reenableWindows = true; // No more modal windows, re-enable any remaining window
				}

				if(reenableWindows)
				{
					Vector<RenderWindowCore*> renderWindows = RenderWindowCoreManager::instance().getRenderWindows();
					for(auto& renderWindow : renderWindows)
					{
						HWND curHwnd;
						renderWindow->getCustomAttribute("WINDOW", &curHwnd);
						EnableWindow(curHwnd, TRUE);
					}
				}

				return 0;
			}
		case WM_SETFOCUS:
			{
				if (!win->getProperties().hasFocus())
					win->_windowFocusReceived();

				return 0;
			}
		case WM_KILLFOCUS:
			{
				if (win->getProperties().hasFocus())
					win->_windowFocusLost();

				return 0;
			}
		case WM_SYSCHAR:
			// return zero to bypass defProc and signal we processed the message, unless it's an ALT-space
			if (wParam != VK_SPACE)
				return 0;
			break;
		case WM_MOVE:
			win->_windowMovedOrResized();
			return 0;
		case WM_DISPLAYCHANGE:
			win->_windowMovedOrResized();
			break;
		case WM_SIZE:
			win->_windowMovedOrResized();

			if (wParam == SIZE_MAXIMIZED)
				win->_notifyMaximized();
			else if (wParam == SIZE_MINIMIZED)
				win->_notifyMinimized();
			else if (wParam == SIZE_RESTORED)
				win->_notifyRestored();

			return 0;
		case WM_SETCURSOR:
			if(isCursorHidden())
				SetCursor(nullptr);
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

				SetCursor(mData->mCursor.cursor);
			}
			return true;
		case WM_GETMINMAXINFO:
		{
			// Prevent the window from going smaller than some minimu size
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 100;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 100;

			// Ensure maximizes window has proper size and doesn't cover the entire screen
			const POINT ptZero = { 0, 0 };
			HMONITOR primaryMonitor = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

			MONITORINFO monitorInfo;
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(primaryMonitor, &monitorInfo);

			((MINMAXINFO*)lParam)->ptMaxPosition.x = monitorInfo.rcWork.left - monitorInfo.rcMonitor.left;
			((MINMAXINFO*)lParam)->ptMaxPosition.y = monitorInfo.rcWork.top - monitorInfo.rcMonitor.top;
			((MINMAXINFO*)lParam)->ptMaxSize.x = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
			((MINMAXINFO*)lParam)->ptMaxSize.y = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;
		}
			break;
		case WM_CLOSE:
			{
				gCoreApplication().quitRequested();

				return 0;
			}
		case WM_NCHITTEST:
			{
				auto iterFind = mData->mNonClientAreas.find(win);
				if (iterFind == mData->mNonClientAreas.end())
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

				Vector<Rect2I>& moveAreasPerWindow = iterFind->second.moveAreas;
				for(auto area : moveAreasPerWindow)
				{
					if(area.contains(mousePosInt))
						return HTCAPTION;
				}

				return HTCLIENT;
			}
		case WM_NCLBUTTONDBLCLK:
			// Maximize/Restore on double-click
			if (wParam == HTCAPTION)
			{
				WINDOWPLACEMENT windowPlacement;
				windowPlacement.length = sizeof(WINDOWPLACEMENT);
				GetWindowPlacement(hWnd, &windowPlacement);

				if (windowPlacement.showCmd == SW_MAXIMIZE)
					ShowWindow(hWnd, SW_RESTORE);
				else
					ShowWindow(hWnd, SW_MAXIMIZE);

				return 0;
			}
			break;
		case WM_MOUSELEAVE:
			{
				// Note: Right now I track only mouse leaving client area. So it's possible for the "mouse left window" callback
				// to trigger, while the mouse is still in the non-client area of the window.
				mData->mIsTrackingMouse = false; // TrackMouseEvent ends when this message is received and needs to be re-applied

				BS_LOCK_MUTEX(mData->mSync);

				if (!onMouseLeftWindow.empty())
					onMouseLeftWindow(win);
			}
			return 0;
		case WM_LBUTTONUP:
			{
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Left, btnStates);

				return 0;
			}
		case WM_MBUTTONUP:
			{
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Middle, btnStates);

				return 0;
			}
		case WM_RBUTTONUP:
			{
				ReleaseCapture();

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonReleased.empty())
					onCursorButtonReleased(intMousePos, OSMouseButton::Right, btnStates);

				return 0;
			}
		case WM_LBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Left, btnStates);
			}
			return 0;
		case WM_MBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Middle, btnStates);
			}
			return 0;
		case WM_RBUTTONDOWN:
			{
				SetCapture(hWnd);

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorButtonPressed.empty())
					onCursorButtonPressed(intMousePos, OSMouseButton::Right, btnStates);
			}
			return 0;
		case WM_LBUTTONDBLCLK:
			{
				Vector2I intMousePos;
				OSPointerButtonStates btnStates;

				getMouseData(hWnd, wParam, lParam, false, intMousePos, btnStates);

				if(!onCursorDoubleClick.empty())
					onCursorDoubleClick(intMousePos, btnStates);
			}
			return 0;
		case WM_NCMOUSEMOVE:
		case WM_MOUSEMOVE:
			{
				// Set up tracking so we get notified when mouse leaves the window
				if(!mData->mIsTrackingMouse)
				{
					TRACKMOUSEEVENT tme = { sizeof(tme) };
					tme.dwFlags = TME_LEAVE;

					tme.hwndTrack = hWnd;
					TrackMouseEvent(&tme);

					mData->mIsTrackingMouse = true;
				}

				Vector2I intMousePos;
				OSPointerButtonStates btnStates;
				
				getMouseData(hWnd, wParam, lParam, uMsg == WM_NCMOUSEMOVE, intMousePos, btnStates);

				if(!onCursorMoved.empty())
					onCursorMoved(intMousePos, btnStates);

				return 0;
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

				return 0;
			}
		case WM_CHAR:
			{
				// TODO - Not handling IME input

				// Ignore rarely used special command characters, usually triggered by ctrl+key
				// combinations. (We want to keep ctrl+key free for shortcuts instead)
				if (wParam <= 23)
					break;

				switch (wParam) 
				{ 
				case VK_ESCAPE:
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
		case WM_BS_SETCAPTURE:
			SetCapture(hWnd);
			break;
		case WM_BS_RELEASECAPTURE:
			ReleaseCapture();
			break;
		case WM_CAPTURECHANGED:
			if(!onMouseCaptureChanged.empty())
				onMouseCaptureChanged();
			return 0;
		}

		return DefWindowProc( hWnd, uMsg, wParam, lParam );
	}
}
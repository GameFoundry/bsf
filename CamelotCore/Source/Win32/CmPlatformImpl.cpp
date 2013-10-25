#include "CmPlatform.h"
#include "CmRenderWindow.h"
#include "CmPixelUtil.h"
#include "CmApplication.h"
#include "CmWin32Defs.h"

namespace CamelotFramework
{
	boost::signal<void(RenderWindow*)> Platform::onMouseLeftWindow;

	boost::signal<void(const Int2&, OSPositionalInputButtonStates)> Platform::onCursorMoved;
	boost::signal<void(const Int2&, OSMouseButton button, OSPositionalInputButtonStates)> Platform::onCursorButtonPressed;
	boost::signal<void(const Int2&, OSMouseButton button, OSPositionalInputButtonStates)> Platform::onCursorButtonReleased;
	boost::signal<void(const Int2&, OSPositionalInputButtonStates)> Platform::onCursorDoubleClick;
	boost::signal<void(InputCommandType)> Platform::onInputCommand;
	boost::signal<void(float)> Platform::onMouseWheelScrolled;
	boost::signal<void(UINT32)> Platform::onCharInput;

	boost::signal<void(RenderWindow*)> Platform::onWindowFocusReceived;
	boost::signal<void(RenderWindow*)> Platform::onWindowFocusLost;
	boost::signal<void(RenderWindow*)> Platform::onWindowMovedOrResized;
	boost::signal<void()> Platform::onMouseCaptureChanged;

	Map<const RenderWindow*, WindowNonClientAreaData>::type Platform::mNonClientAreas;
	bool Platform::mIsTrackingMouse = false;
	Vector<RenderWindow*>::type Platform::mMouseLeftWindows;

	CM_STATIC_MUTEX_CLASS_INSTANCE(mSync, Platform);

	struct NativeCursorData::Pimpl
	{
		HCURSOR cursor;
	};

	NativeCursorData::NativeCursorData()
	{
		data = cm_new<Pimpl>();
	}

	NativeCursorData::~NativeCursorData()
	{
		cm_delete(data);
	}

	bool Platform::mIsCursorHidden = false;
	NativeCursorData Platform::mCursor;
	bool Platform::mUsingCustomCursor = false;

	void Platform::setCursorPosition(const Int2& screenPos)
	{
		SetCursorPos(screenPos.x, screenPos.y);
	}

	void Platform::captureMouse(const RenderWindow& window)
	{
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);
		
		PostMessage(hwnd, WM_CM_SETCAPTURE, WPARAM(hwnd), 0);
	}

	void Platform::releaseMouseCapture()
	{
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_CM_RELEASECAPTURE, WPARAM(hwnd), 0);
	}

	bool Platform::isPointOverWindow(const RenderWindow& window, const Int2& screenPos)
	{
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();

		POINT point;
		point.x = screenPos.x;
		point.y = screenPos.y;

		HWND hwndToCheck;
		window.getCustomAttribute("WINDOW", &hwndToCheck);

		HWND hwndUnderPos = WindowFromPoint(point);
		return hwndUnderPos == hwndToCheck;
	}

	void Platform::hideCursor()
	{
		mIsCursorHidden = true;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::showCursor()
	{
		mIsCursorHidden = false;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::clipCursorToWindow(const RenderWindow& window)
	{
		HWND hwnd;
		window.getCustomAttribute("WINDOW", &hwnd);

		// Clip cursor to the window
		RECT clipWindowRect;
		if(GetWindowRect(hwnd, &clipWindowRect))
		{
			ClipCursor(&clipWindowRect);
		}
	}

	void Platform::clipCursorToRect(const Rect& screenRect)
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

	void Platform::setCursor(CursorType type)
	{
		if(mUsingCustomCursor)
		{
			SetCursor(0);
			DestroyIcon(mCursor.data->cursor);
			mUsingCustomCursor = false;
		}

		switch(type)
		{
		case CursorType::Arrow:
			mCursor.data->cursor = LoadCursor(0, IDC_ARROW);
			break;
		case CursorType::Wait:
			mCursor.data->cursor = LoadCursor(0, IDC_WAIT);
			break;
		case CursorType::IBeam:
			mCursor.data->cursor = LoadCursor(0, IDC_IBEAM);
			break;
		case CursorType::Help:
			mCursor.data->cursor = LoadCursor(0, IDC_HELP);
			break;
		case CursorType::Hand:
			mCursor.data->cursor = LoadCursor(0, IDC_HAND);
			break;
		case CursorType::SizeAll:
			mCursor.data->cursor = LoadCursor(0, IDC_SIZEALL);
			break;
		case CursorType::SizeNESW:
			mCursor.data->cursor = LoadCursor(0, IDC_SIZENESW);
			break;
		case CursorType::SizeNS:
			mCursor.data->cursor = LoadCursor(0, IDC_SIZENS);
			break;
		case CursorType::SizeNWSE:
			mCursor.data->cursor = LoadCursor(0, IDC_SIZENWSE);
			break;
		case CursorType::SizeWE:
			mCursor.data->cursor = LoadCursor(0, IDC_SIZEWE);
			break;
		}

		// Make sure we notify the message loop to perform the actual cursor update
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	// TODO - Add support for animated custom cursor
	void Platform::setCustomCursor(PixelData& pixelData, const Int2& hotSpot)
	{
		if(mUsingCustomCursor)
		{
			SetCursor(0);
			DestroyIcon(mCursor.data->cursor);
		}

		mUsingCustomCursor = true;

		BITMAPV5HEADER bi;

		ZeroMemory(&bi,sizeof(BITMAPV5HEADER));
		bi.bV5Size = sizeof(BITMAPV5HEADER);
		bi.bV5Width = pixelData.getWidth();
		bi.bV5Height = pixelData.getHeight();
		bi.bV5Planes = 1;
		bi.bV5BitCount = 32;
		bi.bV5Compression = BI_BITFIELDS;
		bi.bV5RedMask   =  0x00FF0000;
		bi.bV5GreenMask =  0x0000FF00;
		bi.bV5BlueMask  =  0x000000FF;
		bi.bV5AlphaMask =  0xFF000000; 

		HDC hDC = GetDC(NULL);

		void* data = nullptr;
		HBITMAP hBitmap = CreateDIBSection(hDC, (BITMAPINFO *)&bi, DIB_RGB_COLORS, 
			(void**)&data, NULL, (DWORD)0);

		HDC hBitmapDC = CreateCompatibleDC(hDC); 
		ReleaseDC(NULL, hDC);

		// Create an empty mask bitmap.
		HBITMAP hMonoBitmap = CreateBitmap(pixelData.getWidth(), pixelData.getHeight(), 1, 1, NULL);

		//Select the bitmaps to DC
		HBITMAP hOldBitmap  = (HBITMAP)SelectObject(hBitmapDC, hBitmap);

		//Scan each pixel of the source bitmap and create the masks
		Color pixel;
		DWORD *dst = (DWORD*)data;
		for(UINT32 y = 0; y < pixelData.getHeight(); ++y)
		{
			for(UINT32 x = 0; x < pixelData.getWidth(); ++x)
			{
				pixel = pixelData.getColorAt(x, pixelData.getHeight() - y - 1);
				*dst = pixel.getAsBGRA();

				dst++;
			}
		}

		SelectObject(hBitmapDC, hOldBitmap);

		DeleteDC(hBitmapDC);

		ICONINFO iconinfo = {0};
		iconinfo.fIcon = FALSE;
		iconinfo.xHotspot = (DWORD)hotSpot.x;
		iconinfo.yHotspot = (DWORD)hotSpot.y;
		iconinfo.hbmMask = hMonoBitmap;
		iconinfo.hbmColor = hBitmap;

		mCursor.data->cursor = CreateIconIndirect(&iconinfo);

		DeleteObject(hBitmap);          
		DeleteObject(hMonoBitmap);

		// Make sure we notify the message loop to perform the actual cursor update
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::setCaptionNonClientAreas(const RenderWindow& window, const Vector<Rect>::type& nonClientAreas)
	{
		CM_LOCK_MUTEX(mSync);

		mNonClientAreas[&window].moveAreas = nonClientAreas;
	}

	void Platform::setResizeNonClientAreas(const RenderWindow& window, const Vector<NonClientResizeArea>::type& nonClientAreas)
	{
		CM_LOCK_MUTEX(mSync);

		mNonClientAreas[&window].resizeAreas = nonClientAreas;
	}

	void Platform::resetNonClientAreas(const RenderWindow& window)
	{
		CM_LOCK_MUTEX(mSync);

		auto iterFind = mNonClientAreas.find(&window);

		if(iterFind != end(mNonClientAreas))
			mNonClientAreas.erase(iterFind);
	}

	void Platform::copyToClipboard(const WString& string)
	{
		HANDLE hData = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, (string.size() + 1) * sizeof(WString::value_type));
		WString::value_type* buffer = (WString::value_type*)GlobalLock(hData);

		string.copy(buffer, string.size());
		buffer[string.size()] = '\0';

		GlobalUnlock(hData);

		if(OpenClipboard(NULL))
		{
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hData);
			CloseClipboard();
		}
		else
		{
			GlobalFree(hData);
		}
	}

	WString Platform::copyFromClipboard()
	{
		if(OpenClipboard(NULL))
		{
			HANDLE hData = GetClipboardData(CF_UNICODETEXT);

			if(hData != NULL)
			{
				WString::value_type* buffer = (WString::value_type*)GlobalLock(hData);
				WString string(buffer);
				GlobalUnlock(hData);

				CloseClipboard();
				return string;
			}
			else
			{
				CloseClipboard();
				return L"";
			}			
		}

		return L"";
	}

	double Platform::queryPerformanceTimerMs()
	{
		LARGE_INTEGER counterValue;
		QueryPerformanceCounter(&counterValue);
		
		LARGE_INTEGER counterFreq;
		QueryPerformanceFrequency(&counterFreq);

		return (double)counterValue.QuadPart / (counterFreq.QuadPart * 0.001);
	}

	void Platform::messagePump()
	{
		MSG  msg;
		while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Platform::update()
	{
		Vector<RenderWindow*>::type windowsCopy;
		{
			CM_LOCK_MUTEX(mSync);

			windowsCopy = mMouseLeftWindows;
			mMouseLeftWindows.clear();
		}
		
		for(auto& window : windowsCopy)
		{
			if(!onMouseLeftWindow.empty())
				onMouseLeftWindow(window);
		}
	}

	void Platform::windowFocusReceived(RenderWindow* window)
	{
		if(!onWindowFocusReceived.empty())
			onWindowFocusReceived(window);
	}

	void Platform::windowFocusLost(RenderWindow* window)
	{
		if(!onWindowFocusLost.empty())
			onWindowFocusLost(window);
	}
	
	void Platform::windowMovedOrResized(RenderWindow* window)
	{
		if(!onWindowMovedOrResized.empty())
			onWindowMovedOrResized(window);
	}

	void Platform::win32ShowCursor()
	{
		SetCursor(mCursor.data->cursor);
	}

	void Platform::win32HideCursor()
	{
		SetCursor(nullptr);
	}
}
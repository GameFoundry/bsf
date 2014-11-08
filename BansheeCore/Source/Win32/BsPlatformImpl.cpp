#include "BsPlatform.h"
#include "BsRenderWindow.h"
#include "BsPixelUtil.h"
#include "BsCoreApplication.h"
#include "BsDebug.h"
#include "Win32/BsWin32Defs.h"
#include "Win32/BsWin32DropTarget.h"
#include <iphlpapi.h>

namespace BansheeEngine
{
	Event<void(RenderWindow*)> Platform::onMouseLeftWindow;

	Event<void(const Vector2I&, OSPointerButtonStates)> Platform::onCursorMoved;
	Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> Platform::onCursorButtonPressed;
	Event<void(const Vector2I&, OSMouseButton button, OSPointerButtonStates)> Platform::onCursorButtonReleased;
	Event<void(const Vector2I&, OSPointerButtonStates)> Platform::onCursorDoubleClick;
	Event<void(InputCommandType)> Platform::onInputCommand;
	Event<void(float)> Platform::onMouseWheelScrolled;
	Event<void(UINT32)> Platform::onCharInput;

	Event<void(RenderWindowCore*)> Platform::onWindowFocusReceived;
	Event<void(RenderWindowCore*)> Platform::onWindowFocusLost;
	Event<void(RenderWindowCore*)> Platform::onWindowMovedOrResized;
	Event<void()> Platform::onMouseCaptureChanged;

	Map<const RenderWindowCore*, WindowNonClientAreaData> Platform::mNonClientAreas;
	bool Platform::mIsTrackingMouse = false;
	Vector<RenderWindowCore*> Platform::mMouseLeftWindows;

	Stack<RenderWindowCore*> Platform::mModalWindowStack;

	NativeDropTargetData Platform::mDropTargets;

	bool Platform::mRequiresStartUp = false;
	bool Platform::mRequiresShutDown = false;

	BS_STATIC_MUTEX_CLASS_INSTANCE(mSync, Platform);

	struct NativeCursorData::Pimpl
	{
		HCURSOR cursor;
	};

	NativeCursorData::NativeCursorData()
	{
		data = bs_new<Pimpl>();
	}

	NativeCursorData::~NativeCursorData()
	{
		bs_delete(data);
	}

	struct NativeDropTargetData::Pimpl
	{
		Map<const RenderWindow*, Win32DropTarget*> dropTargetsPerWindow;
		Vector<Win32DropTarget*> dropTargetsToInitialize;
		Vector<Win32DropTarget*> dropTargetsToDestroy;
	};

	NativeDropTargetData::NativeDropTargetData()
	{
		data = bs_new<Pimpl>();
	}

	NativeDropTargetData::~NativeDropTargetData()
	{
		bs_delete(data);
	}

	bool Platform::mIsCursorHidden = false;
	NativeCursorData Platform::mCursor;
	bool Platform::mUsingCustomCursor = false;

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
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);
		
		PostMessage(hwnd, WM_BS_SETCAPTURE, WPARAM(hwnd), 0);
	}

	void Platform::releaseMouseCapture()
	{
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_BS_RELEASECAPTURE, WPARAM(hwnd), 0);
	}

	bool Platform::isPointOverWindow(const RenderWindow& window, const Vector2I& screenPos)
	{
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();

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

		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::showCursor()
	{
		mIsCursorHidden = false;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
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
		RenderWindowPtr primaryWindow = gCoreApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Platform::setCaptionNonClientAreas(const RenderWindow& window, const Vector<Rect2I>& nonClientAreas)
	{
		BS_LOCK_MUTEX(mSync);

		mNonClientAreas[window.getCore().get()].moveAreas = nonClientAreas;
	}

	void Platform::setResizeNonClientAreas(const RenderWindow& window, const Vector<NonClientResizeArea>& nonClientAreas)
	{
		BS_LOCK_MUTEX(mSync);

		mNonClientAreas[window.getCore().get()].resizeAreas = nonClientAreas;
	}

	void Platform::resetNonClientAreas(const RenderWindow& window)
	{
		BS_LOCK_MUTEX(mSync);

		auto iterFind = mNonClientAreas.find(window.getCore().get());

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

	bool Platform::getMACAddress(MACAddress& address)
	{
		std::memset(&address, 0, sizeof(address));

		PIP_ADAPTER_INFO adapterInfo = bs_alloc<IP_ADAPTER_INFO>();
		
		ULONG len = sizeof(IP_ADAPTER_INFO);
		DWORD rc = GetAdaptersInfo(adapterInfo, &len);

		if (rc == ERROR_BUFFER_OVERFLOW)
		{
			bs_free(adapterInfo);
			adapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(bs_alloc(len));
		}
		else if (rc != ERROR_SUCCESS)
		{
			bs_free(adapterInfo);
			return false;
		}

		if (GetAdaptersInfo(adapterInfo, &len) == NO_ERROR)
		{
			PIP_ADAPTER_INFO curAdapter = nullptr;
			curAdapter = adapterInfo;

			while (curAdapter)
			{
				if (curAdapter->Type == MIB_IF_TYPE_ETHERNET && curAdapter->AddressLength == sizeof(address))
				{
					std::memcpy(&address, curAdapter->Address, curAdapter->AddressLength);
					return true;
				}

				curAdapter = curAdapter->Next;
			}
		}

		bs_free(adapterInfo);
		return false;
	}

	double Platform::queryPerformanceTimerMs()
	{
		LARGE_INTEGER counterValue;
		QueryPerformanceCounter(&counterValue);
		
		LARGE_INTEGER counterFreq;
		QueryPerformanceFrequency(&counterFreq);

		return (double)counterValue.QuadPart / (counterFreq.QuadPart * 0.001);
	}

	OSDropTarget& Platform::createDropTarget(const RenderWindow* window, int x, int y, unsigned int width, unsigned int height)
	{
		Win32DropTarget* win32DropTarget = nullptr;
		auto iterFind = mDropTargets.data->dropTargetsPerWindow.find(window);
		if(iterFind == mDropTargets.data->dropTargetsPerWindow.end())
		{
			HWND hwnd;
			window->getCustomAttribute("WINDOW", &hwnd);

			win32DropTarget = bs_new<Win32DropTarget>(hwnd);
			mDropTargets.data->dropTargetsPerWindow[window] = win32DropTarget;

			{
				BS_LOCK_MUTEX(mSync);
				mDropTargets.data->dropTargetsToInitialize.push_back(win32DropTarget);
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
		auto iterFind = mDropTargets.data->dropTargetsPerWindow.find(target.getOwnerWindow());
		if(iterFind == mDropTargets.data->dropTargetsPerWindow.end())
		{
			LOGWRN("Attempting to destroy a drop target but cannot find its parent window.");
		}
		else
		{
			Win32DropTarget* win32DropTarget = iterFind->second;
			win32DropTarget->unregisterDropTarget(&target);

			if(win32DropTarget->getNumDropTargets() == 0)
			{
				mDropTargets.data->dropTargetsPerWindow.erase(iterFind);

				{
					BS_LOCK_MUTEX(mSync);
					mDropTargets.data->dropTargetsToDestroy.push_back(win32DropTarget);
				}
			}
		}
		
		BS_PVT_DELETE(OSDropTarget, &target);
	}

	void Platform::_messagePump()
	{
		MSG  msg;
		while(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void Platform::_startUp()
	{
		BS_LOCK_MUTEX(mSync);

		mRequiresStartUp = true;
	}

	void Platform::_update()
	{
		Vector<RenderWindowCore*> windowsCopy;
		{
			BS_LOCK_MUTEX(mSync);

			windowsCopy = mMouseLeftWindows;
			mMouseLeftWindows.clear();
		}
		
		for(auto& window : windowsCopy)
		{
			if(!onMouseLeftWindow.empty())
				onMouseLeftWindow(window->getNonCore());
		}

		for(auto& dropTarget : mDropTargets.data->dropTargetsPerWindow)
		{
			dropTarget.second->update();
		}
	}

	void Platform::_coreUpdate()
	{
		{
			BS_LOCK_MUTEX(mSync);
			if(mRequiresStartUp)
			{
				OleInitialize(nullptr);

				mRequiresStartUp = false;
			}
		}

		{
			BS_LOCK_MUTEX(mSync);
			for(auto& dropTargetToInit : mDropTargets.data->dropTargetsToInitialize)
			{
				dropTargetToInit->registerWithOS();
			}

			mDropTargets.data->dropTargetsToInitialize.clear();
		}

		{
			BS_LOCK_MUTEX(mSync);
			for(auto& dropTargetToDestroy : mDropTargets.data->dropTargetsToDestroy)
			{
				dropTargetToDestroy->unregisterWithOS();
				dropTargetToDestroy->Release();
			}

			mDropTargets.data->dropTargetsToDestroy.clear();
		}

		_messagePump();

		{
			BS_LOCK_MUTEX(mSync);
			if(mRequiresShutDown)
			{
				OleUninitialize();
				mRequiresShutDown = false;
			}
		}
	}

	void Platform::_shutDown()
	{
		BS_LOCK_MUTEX(mSync);

		mRequiresShutDown = true;
	}

	void Platform::windowFocusReceived(RenderWindowCore* window)
	{
		if(!onWindowFocusReceived.empty())
			onWindowFocusReceived(window);
	}

	void Platform::windowFocusLost(RenderWindowCore* window)
	{
		if(!onWindowFocusLost.empty())
			onWindowFocusLost(window);
	}
	
	void Platform::windowMovedOrResized(RenderWindowCore* window)
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
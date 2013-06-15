#include "CmCursor.h"
#include "CmRenderWindow.h"
#include "CmWindowEventUtilities.h"
#include "CmPixelUtil.h"
#include "CmApplication.h"
#include <windows.h>

namespace CamelotFramework
{
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

	bool Cursor::mIsHidden = false;
	NativeCursorData Cursor::mCursor;
	bool Cursor::mUsingCustom = false;

	void Cursor::setPosition(const Int2& screenPos)
	{
		SetCursorPos(screenPos.x, screenPos.y);
	}

	void Cursor::hide()
	{
		mIsHidden = true;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Cursor::show()
	{
		mIsHidden = false;

		// ShowCursor(FALSE) doesn't work. Presumably because we're in the wrong thread, and using
		// WM_SETCURSOR in message loop to hide the cursor is smarter solution anyway.

		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}

	void Cursor::clipToWindow(const RenderWindow& window)
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

	void Cursor::clipToRect(const Rect& screenRect)
	{
		RECT clipWindowRect;
		clipWindowRect.left = screenRect.x;
		clipWindowRect.top = screenRect.y;
		clipWindowRect.right = screenRect.x + screenRect.width;
		clipWindowRect.bottom = screenRect.y + screenRect.height;

		ClipCursor(&clipWindowRect);
	}

	void Cursor::clipDisable()
	{
		ClipCursor(NULL);
	}

	void Cursor::setCursor(CursorType type)
	{
		if(mUsingCustom)
		{
			SetCursor(0);
			DestroyIcon(mCursor.data->cursor);
			mUsingCustom = false;
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
	void Cursor::setCustomCursor(PixelData& pixelData, const Int2& hotSpot)
	{
		if(mUsingCustom)
		{
			SetCursor(0);
			DestroyIcon(mCursor.data->cursor);
		}

		mUsingCustom = true;

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

	void Cursor::_win32ShowCursor()
	{
		SetCursor(mCursor.data->cursor);
	}

	void Cursor::_win32HideCursor()
	{
		SetCursor(nullptr);
	}
}
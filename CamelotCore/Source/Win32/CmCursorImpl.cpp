#include "CmCursor.h"
#include "CmRenderWindow.h"
#include "CmWindowEventUtilities.h"
#include "CmPixelUtil.h"
#include "CmApplication.h"

namespace CamelotFramework
{
	bool Cursor::mIsHidden = false;
	HCURSOR Cursor::mCursor;
	bool Cursor::mUsingCustom = false;

	Int2 Cursor::getWindowPosition(const RenderWindow& window)
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		HWND hwnd;
		window.getCustomAttribute("WINDOW", &hwnd);

		ScreenToClient(hwnd, &screenPos);

		return Int2(screenPos.x, screenPos.y);
	}

	void Cursor::setWindowPosition(const RenderWindow& window, const Int2& pos)
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

	Int2 Cursor::getScreenPosition()
	{
		POINT screenPos;
		GetCursorPos(&screenPos);

		return Int2(screenPos.x, screenPos.y);
	}

	void Cursor::setScreenPosition(const Int2& pos)
	{
		POINT screenPos;

		screenPos.x = pos.x;
		screenPos.y = pos.y;

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
			DestroyIcon(mCursor);
			mUsingCustom = false;
		}

		switch(type)
		{
		case CursorType::Arrow:
			mCursor = LoadCursor(0, IDC_ARROW);
			break;
		case CursorType::Wait:
			mCursor = LoadCursor(0, IDC_WAIT);
			break;
		case CursorType::IBeam:
			mCursor = LoadCursor(0, IDC_IBEAM);
			break;
		case CursorType::Help:
			mCursor = LoadCursor(0, IDC_HELP);
			break;
		case CursorType::Hand:
			mCursor = LoadCursor(0, IDC_HAND);
			break;
		case CursorType::SizeAll:
			mCursor = LoadCursor(0, IDC_SIZEALL);
			break;
		case CursorType::SizeNESW:
			mCursor = LoadCursor(0, IDC_SIZENESW);
			break;
		case CursorType::SizeNS:
			mCursor = LoadCursor(0, IDC_SIZENS);
			break;
		case CursorType::SizeNWSE:
			mCursor = LoadCursor(0, IDC_SIZENWSE);
			break;
		case CursorType::SizeWE:
			mCursor = LoadCursor(0, IDC_SIZEWE);
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
			DestroyIcon(mCursor);
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

		mCursor = CreateIconIndirect(&iconinfo);

		DeleteObject(hBitmap);          
		DeleteObject(hMonoBitmap);

		// Make sure we notify the message loop to perform the actual cursor update
		RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		HWND hwnd;
		primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));


		//HDC hDC = GetDC(NULL);
		//HDC hAndMaskDC = CreateCompatibleDC(hDC); 
		//HDC hXorMaskDC = CreateCompatibleDC(hDC); 

		////Get the dimensions of the source bitmap
		//HBITMAP hAndMaskBitmap  = CreateCompatibleBitmap(hDC, pixelData.getWidth(), pixelData.getHeight());
		//HBITMAP hXorMaskBitmap  = CreateCompatibleBitmap(hDC, pixelData.getWidth(), pixelData.getHeight());

		////Select the bitmaps to DC
		//HBITMAP hOldAndMaskBitmap  = (HBITMAP)SelectObject(hAndMaskDC, hAndMaskBitmap);
		//HBITMAP hOldXorMaskBitmap  = (HBITMAP)SelectObject(hXorMaskDC, hXorMaskBitmap);

		////Scan each pixel of the souce bitmap and create the masks
		//Color pixel;
		//for(UINT32 x = 0; x < pixelData.getWidth(); ++x)
		//{
		//	for(UINT32 y = 0; y < pixelData.getHeight(); ++y)
		//	{
		//		pixel = pixelData.getColorAt(x, y);
		//		DWORD pixel32 = pixel.getAsRGBA();

		//		if(pixel.a < 1.0f)
		//		{
		//			SetPixel(hAndMaskDC, x, y, RGB(255,255,255));
		//			/*SetPixel(hXorMaskDC, x, y, RGB(pixel32 & 0xFF, (pixel32 >> 8) & 0xFF, (pixel32 >> 16) & 0xFF));*/
		//			SetPixel(hXorMaskDC, x, y, RGB(255,255,255));
		//		}
		//		else
		//		{
		//			SetPixel(hAndMaskDC, x, y, RGB(0,0,0));
		//			//SetPixel(hXorMaskDC, x, y, RGB(pixel32 & 0xFF, (pixel32 >> 8) & 0xFF, (pixel32 >> 16) & 0xFF));
		//			SetPixel(hXorMaskDC, x, y, RGB(255,255,255));
		//		}
		//	}
		//}

		//SelectObject(hAndMaskDC, hOldAndMaskBitmap);
		//SelectObject(hXorMaskDC, hOldXorMaskBitmap);

		//DeleteDC(hXorMaskDC);
		//DeleteDC(hAndMaskDC);

		//ReleaseDC(NULL, hDC);

		//ICONINFO iconinfo = {0};
		//iconinfo.fIcon = FALSE;
		//iconinfo.xHotspot = (DWORD)hotSpot.x;
		//iconinfo.yHotspot = (DWORD)hotSpot.y;
		//iconinfo.hbmMask = hAndMaskBitmap;
		//iconinfo.hbmColor = hXorMaskBitmap;

		//mCursor = CreateIconIndirect(&iconinfo);

		//// Make sure we notify the message loop to perform the actual cursor update
		//RenderWindowPtr primaryWindow = gApplication().getPrimaryWindow();
		//HWND hwnd;
		//primaryWindow->getCustomAttribute("WINDOW", &hwnd);

		//PostMessage(hwnd, WM_SETCURSOR, WPARAM(hwnd), (LPARAM)MAKELONG(HTCLIENT, WM_MOUSEMOVE));
	}
}
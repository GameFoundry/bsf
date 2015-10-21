#include "Win32/BsWin32Window.h"
#include "Win32/BsWin32Platform.h"

namespace BansheeEngine
{
	struct Win32Window::Pimpl
	{
		HWND hWnd = nullptr;
		INT32 left = 0;
		INT32 top = 0;
		UINT32 width = 0;
		UINT32 height = 0;
		bool isExternal = false;
	};

	Win32Window::Win32Window(const WINDOW_DESC& desc)
	{
		m = bs_new<Pimpl>();

		HMONITOR hMonitor = desc.monitor;
		if (!desc.external)
		{
			DWORD dwStyle = WS_CLIPCHILDREN;

			if (!desc.hidden)
				dwStyle |= WS_VISIBLE;

			DWORD dwStyleEx = 0;
			RECT rc;
			MONITORINFO monitorInfo;

			INT32 left = desc.left;
			INT32 top = desc.top;

			// If we didn't specified the adapter index, or if we didn't find it
			if (hMonitor == nullptr)
			{
				POINT windowAnchorPoint;

				// Fill in anchor point.
				windowAnchorPoint.x = left;
				windowAnchorPoint.y = top;

				// Get the nearest monitor to this window.
				hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
			}

			// Get the target monitor info		
			memset(&monitorInfo, 0, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);

			UINT32 width = desc.width;
			UINT32 height = desc.height;

			// No specified top left -> Center the window in the middle of the monitor
			if (left == -1 || top == -1)
			{
				int screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
				int screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

				// clamp window dimensions to screen size
				int outerw = (int(width) < screenw) ? int(width) : screenw;
				int outerh = (int(height) < screenh) ? int(height) : screenh;

				if (left == -1)
					left = monitorInfo.rcWork.left + (screenw - outerw) / 2;
				else if (hMonitor != nullptr)
					left += monitorInfo.rcWork.left;

				if (top == -1)
					top = monitorInfo.rcWork.top + (screenh - outerh) / 2;
				else if (hMonitor != nullptr)
					top += monitorInfo.rcWork.top;
			}
			else if (hMonitor != nullptr)
			{
				left += monitorInfo.rcWork.left;
				top += monitorInfo.rcWork.top;
			}

			if (!desc.fullscreen)
			{
				if (desc.parent)
				{
					if (desc.toolWindow)
						dwStyleEx = WS_EX_TOOLWINDOW;
					else
						dwStyle |= WS_CHILD;
				}

				if (!desc.parent || desc.toolWindow)
				{
					if (desc.border == WindowBorder::None)
						dwStyle |= WS_POPUP;
					else if (desc.border == WindowBorder::Fixed)
						dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
						WS_SYSMENU | WS_MINIMIZEBOX;
					else
						dwStyle |= WS_OVERLAPPEDWINDOW;
				}

				if (!desc.outerDimensions)
				{
					// Calculate window dimensions required to get the requested client area
					SetRect(&rc, 0, 0, width, height);
					AdjustWindowRect(&rc, dwStyle, false);
					width = rc.right - rc.left;
					height = rc.bottom - rc.top;

					// Clamp width and height to the desktop dimensions
					int screenw = GetSystemMetrics(SM_CXSCREEN);
					int screenh = GetSystemMetrics(SM_CYSCREEN);

					if ((int)width > screenw)
						width = screenw;

					if ((int)height > screenh)
						height = screenh;

					if (left < 0)
						left = (screenw - width) / 2;

					if (top < 0)
						top = (screenh - height) / 2;
				}
			}
			else
			{
				dwStyle |= WS_POPUP;
				top = 0;
				left = 0;
			}

			UINT classStyle = 0;
			if (desc.enableDoubleClick)
				classStyle |= CS_DBLCLKS;

			HINSTANCE hInst = NULL;

			// Register the window class
			WNDCLASS wc = { classStyle, Win32Platform::_win32WndProc, 0, 0, hInst,
				LoadIcon(nullptr, IDI_APPLICATION), LoadCursor(nullptr, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "Win32Wnd" };

			RegisterClass(&wc);

			// Create main window
			m->hWnd = CreateWindowEx(dwStyleEx, "Win32Wnd", desc.title.c_str(), dwStyle,
				left, top, width, height, desc.parent, nullptr, hInst, this);
			m->isExternal = false;
		}
		else
		{
			m->hWnd = desc.external;
			m->isExternal = true;
		}

		RECT rc;
		GetWindowRect(m->hWnd, &rc);
		m->top = rc.top;
		m->left = rc.left;

		GetClientRect(m->hWnd, &rc);
		m->width = rc.right;
		m->height = rc.bottom;
	}

	Win32Window::~Win32Window()
	{
		if (m->hWnd && !m->isExternal)
			DestroyWindow(m->hWnd);

		bs_delete(m);
	}

	void Win32Window::move(INT32 top, INT32 left)
	{
		if (m->hWnd)
		{
			m->top = top;
			m->left = left;

			SetWindowPos(m->hWnd, nullptr, left, top, m->width, m->height, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Win32Window::resize(UINT32 width, UINT32 height)
	{
		if (m->hWnd)
		{
			RECT rc = { m->top, m->left, width, height };
			AdjustWindowRect(&rc, GetWindowLong(m->hWnd, GWL_STYLE), false);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;

			m->width = width;
			m->height = height;

			SetWindowPos(m->hWnd, nullptr, m->left, m->top, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Win32Window::setActive(bool state)
	{
		if (m->hWnd)
		{
			if (state)
				ShowWindow(m->hWnd, SW_RESTORE);
			else
				ShowWindow(m->hWnd, SW_SHOWMINNOACTIVE);
		}
	}

	void Win32Window::setHidden(bool hidden)
	{
		if (hidden)
			ShowWindow(m->hWnd, SW_HIDE);
		else
			ShowWindow(m->hWnd, SW_SHOWNORMAL);
	}

	void Win32Window::minimize()
	{
		if (m->hWnd)
			SendMessage(m->hWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	void Win32Window::maximize()
	{
		if (m->hWnd)
			SendMessage(m->hWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	void Win32Window::restore()
	{
		if (m->hWnd)
			SendMessage(m->hWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	void Win32Window::_windowMovedOrResized()
	{
		if (!m->hWnd || IsIconic(m->hWnd))
			return;

		RECT rc;
		GetWindowRect(m->hWnd, &rc);
		m->top = rc.top;
		m->left = rc.left;

		GetClientRect(m->hWnd, &rc);
		m->width = rc.right - rc.left;
		m->height = rc.bottom - rc.top;
	}

	Vector2I Win32Window::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(m->hWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I Win32Window::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(m->hWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}
}
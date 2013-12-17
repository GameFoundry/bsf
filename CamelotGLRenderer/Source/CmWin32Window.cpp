/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#include "CmWin32Window.h"
#include "CmInput.h"
#include "CmRenderSystem.h"
#include "CmCoreThread.h"
#include "CmException.h"
#include "CmWin32GLSupport.h"
#include "CmWin32Context.h"
#include "CmPlatformWndProc.h"
#include "CmGLPixelFormat.h"

namespace CamelotFramework 
{
	#define _MAX_CLASS_NAME_ 128

	Win32Window::Win32Window(const RENDER_WINDOW_DESC& desc, Win32GLSupport &glsupport):
		RenderWindow(desc),
		mGLSupport(glsupport),
		mContext(0)
	{
		mIsFullScreen = false;
		mHWnd = 0;
		mGlrc = 0;
		mIsExternal = false;
		mIsExternalGLControl = false;
		mIsExternalGLContext = false;
		mSizing = false;
		mClosed = false;
		mDisplayFrequency = 0;
		mActive = false;
		mDeviceName = NULL;
	}

	Win32Window::~Win32Window()
	{

	}

	void Win32Window::initialize_internal()
	{
#ifdef CM_STATIC_LIB
		HINSTANCE hInst = GetModuleHandle( NULL );
#else
		HINSTANCE hInst = GetModuleHandle("CamelotGLRenderSystem.dll");
#endif

		mHWnd = 0;
		mName = mDesc.title;
		mIsFullScreen = mDesc.fullscreen;
		mClosed = false;		
		mDisplayFrequency = mDesc.displayFrequency;
		mColorDepth = mDesc.colorDepth;
		HWND parent = 0;
		HMONITOR hMonitor = NULL;

		int monitorIndex = mDesc.monitorIndex;

		// Get variable-length params
		NameValuePairList::const_iterator opt;
		NameValuePairList::const_iterator end = mDesc.platformSpecific.end();

		if ((opt = mDesc.platformSpecific.find("externalWindowHandle")) != end)
		{
			mHWnd = (HWND)parseUnsignedInt(opt->second);
			if (mHWnd)
			{
				mIsExternal = true;
				mIsFullScreen = false;
			}

			if ((opt = mDesc.platformSpecific.find("externalGLControl")) != end) {
				mIsExternalGLControl = parseBool(opt->second);
			}
		}

		if ((opt = mDesc.platformSpecific.find("externalGLContext")) != end)
		{
			mGlrc = (HGLRC)parseUnsignedLong(opt->second);
			if( mGlrc )
				mIsExternalGLContext = true;
		}

		// incompatible with fullscreen
		if ((opt = mDesc.platformSpecific.find("parentWindowHandle")) != end)
			parent = (HWND)parseUnsignedInt(opt->second);

		// monitor handle
		if ((opt = mDesc.platformSpecific.find("monitorHandle")) != end)
			hMonitor = (HMONITOR)parseInt(opt->second);			

		if (!mIsFullScreen)
		{
			// make sure we don't exceed desktop colour depth
			if ((int)mColorDepth > GetDeviceCaps(GetDC(0), BITSPIXEL))
				mColorDepth = GetDeviceCaps(GetDC(0), BITSPIXEL);
		}

		if (!mIsExternal)
		{
			DWORD		  dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;
			DWORD		  dwStyleEx = 0;					
			MONITORINFOEX monitorInfoEx;
			RECT		  rc;

			// If we didn't specified the adapter index, or if it didn't find it
			if (hMonitor == NULL)
			{
				POINT windowAnchorPoint;

				// Fill in anchor point.
				windowAnchorPoint.x = mDesc.left;
				windowAnchorPoint.y = mDesc.top;


				// Get the nearest monitor to this window.
				hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTONEAREST);
			}

			// Get the target monitor info		
			memset(&monitorInfoEx, 0, sizeof(MONITORINFOEX));
			monitorInfoEx.cbSize = sizeof(MONITORINFOEX);
			GetMonitorInfo(hMonitor, &monitorInfoEx);

			size_t devNameLen = strlen(monitorInfoEx.szDevice);
			mDeviceName = (char*)cm_alloc<ScratchAlloc>((UINT32)(devNameLen + 1));

			strcpy_s(mDeviceName, devNameLen + 1, monitorInfoEx.szDevice);

			UINT32 left = mDesc.left;
			UINT32 top = mDesc.top;

			// No specified top left -> Center the window in the middle of the monitor
			if (left == -1 || top == -1)
			{				
				int screenw = monitorInfoEx.rcWork.right  - monitorInfoEx.rcWork.left;
				int screenh = monitorInfoEx.rcWork.bottom - monitorInfoEx.rcWork.top;

				unsigned int winWidth, winHeight;
				_adjustWindow(mDesc.width, mDesc.height, &winWidth, &winHeight);

				// clamp window dimensions to screen size
				int outerw = (int(winWidth) < screenw)? int(winWidth) : screenw;
				int outerh = (int(winHeight) < screenh)? int(winHeight) : screenh;

				if (left == -1)
					left = monitorInfoEx.rcWork.left + (screenw - outerw) / 2;
				else if (mDesc.monitorIndex != -1)
					left += monitorInfoEx.rcWork.left;

				if (top == -1)
					top = monitorInfoEx.rcWork.top + (screenh - outerh) / 2;
				else if (mDesc.monitorIndex != -1)
					top += monitorInfoEx.rcWork.top;
			}
			else if (mDesc.monitorIndex != -1)
			{
				left += monitorInfoEx.rcWork.left;
				top += monitorInfoEx.rcWork.top;
			}

			mWidth = mDesc.width;
			mHeight = mDesc.height;
			mTop = top;
			mLeft = left;

			if (mIsFullScreen)
			{
				dwStyle |= WS_POPUP;
				dwStyleEx |= WS_EX_TOPMOST;
				mTop = monitorInfoEx.rcMonitor.top;
				mLeft = monitorInfoEx.rcMonitor.left;											
			}
			else
			{				
				if (parent)
				{
					if(mDesc.toolWindow)
						dwStyleEx = WS_EX_TOOLWINDOW;
					else
						dwStyle |= WS_CHILD;
				}

				if (!parent || mDesc.toolWindow)
				{
					if (mDesc.border == WindowBorder::None)
						dwStyle |= WS_POPUP;
					else if (mDesc.border == WindowBorder::Fixed)
						dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
						WS_SYSMENU | WS_MINIMIZEBOX;
					else
						dwStyle |= WS_OVERLAPPEDWINDOW;
				}

				int screenw = GetSystemMetrics(SM_CXSCREEN);
				int screenh = GetSystemMetrics(SM_CYSCREEN);

				if (!mDesc.outerDimensions)
				{
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, mWidth, mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					mWidth = rc.right - rc.left;
					mHeight = rc.bottom - rc.top;

					// Clamp window rect to the nearest display monitor.
					if (mLeft < monitorInfoEx.rcWork.left)
						mLeft = monitorInfoEx.rcWork.left;		

					if (mTop < monitorInfoEx.rcWork.top)					
						mTop = monitorInfoEx.rcWork.top;					

					if ((int)mWidth > monitorInfoEx.rcWork.right - mLeft)					
						mWidth = monitorInfoEx.rcWork.right - mLeft;	

					if ((int)mHeight > monitorInfoEx.rcWork.bottom - mTop)					
						mHeight = monitorInfoEx.rcWork.bottom - mTop;		
				}			
			}

			// register class and create window
			WNDCLASS wc = { CS_OWNDC, PlatformWndProc::_win32WndProc, 0, 0, hInst,
				LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, "GLWindow" };
			RegisterClass(&wc);

			if (mIsFullScreen)
			{
				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = mColorDepth;
				displayDeviceMode.dmPelsWidth = mWidth;
				displayDeviceMode.dmPelsHeight = mHeight;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						// TODO LOG PORT - Log this somewhere
						//LogManager::getSingleton().logMessage(LML_NORMAL, "ChangeDisplaySettings with user display frequency failed");
						//displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
					}
				}
				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					// TODO LOG PORT - Log this somewhere
					//LogManager::getSingleton().logMessage(LML_CRITICAL, "ChangeDisplaySettings failed");
				}
			}

			// Pass pointer to self as WM_CREATE parameter
			mHWnd = CreateWindowEx(dwStyleEx, "GLWindow", mDesc.title.c_str(),
				dwStyle, mLeft, mTop, mWidth, mHeight, parent, 0, hInst, this);		
		}

		RECT rc;
		// top and left represent outer window position
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;
		// width and height represent drawable area only
		GetClientRect(mHWnd, &rc);
		mWidth = rc.right;
		mHeight = rc.bottom;

		mHDC = GetDC(mHWnd);

		if (!mIsExternalGLControl)
		{
			int testFsaa = mFSAA;
			bool testHwGamma = mDesc.gamma;
			bool formatOk = mGLSupport.selectPixelFormat(mHDC, mColorDepth, testFsaa, testHwGamma);
			if (!formatOk)
			{
				if (mFSAA > 0)
				{
					// try without FSAA
					testFsaa = 0;
					formatOk = mGLSupport.selectPixelFormat(mHDC, mColorDepth, testFsaa, testHwGamma);
				}

				if (!formatOk && mDesc.gamma)
				{
					// try without sRGB
					testHwGamma = false;
					testFsaa = mFSAA;
					formatOk = mGLSupport.selectPixelFormat(mHDC, mColorDepth, testFsaa, testHwGamma);
				}

				if (!formatOk && mDesc.gamma && (mFSAA > 0))
				{
					// try without both
					testHwGamma = false;
					testFsaa = 0;
					formatOk = mGLSupport.selectPixelFormat(mHDC, mColorDepth, testFsaa, testHwGamma);
				}

				if (!formatOk)
					CM_EXCEPT(RenderingAPIException, "selectPixelFormat failed");

			}
			// record what gamma option we used in the end
			// this will control enabling of sRGB state flags when used
			mHwGamma = testHwGamma;
			mFSAA = testFsaa;
		}
		
		mActive = true;

		GLRenderSystem* rs = static_cast<GLRenderSystem*>(RenderSystem::instancePtr());

		// If RenderSystem has initialized a context use that, otherwise we create our own
		if(!rs->isContextInitialized())
		{
			if (!mIsExternalGLContext)
			{
#if CM_DEBUG_MODE
				if (wglewIsSupported("WGL_ARB_create_context"))
				{
					int contextAttribs[] =
					{
						WGL_CONTEXT_FLAGS_ARB,
						WGL_CONTEXT_DEBUG_BIT_ARB,
						0
					};

					mGlrc = wglCreateContextAttribsARB(mHDC, 0, contextAttribs);
				}
				else
					mGlrc = wglCreateContext(mHDC);
#else
				mGlrc = wglCreateContext(mHDC);
#endif


				if (!mGlrc)
				{
					CM_EXCEPT(RenderingAPIException, 
						"wglCreateContext failed: " + translateWGLError());
				}
			}
		}
		else
		{
			rs->getMainContext()->setCurrent();
			mGlrc = wglGetCurrentContext();
		}

		// Create RenderSystem context
		mContext = cm_new<Win32Context>(mHDC, mGlrc);

		RenderWindow::initialize_internal();
	}

	void Win32Window::destroy_internal()
	{
		if (!mHWnd)
			return;

		// Unregister and destroy GLContext
		cm_delete(mContext);

		if (!mIsExternalGLContext && mGlrc)
		{
			wglDeleteContext(mGlrc);
			mGlrc = 0;
		}
		if (!mIsExternal)
		{
			if (mIsFullScreen)
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
			DestroyWindow(mHWnd);
		}
		else
		{
			// just release the DC
			ReleaseDC(mHWnd, mHDC);
		}

		mActive = false;
		mClosed = true;
		mHDC = 0; // no release thanks to CS_OWNDC wndclass style
		mHWnd = 0;

		if (mDeviceName != NULL)
		{
			cm_free<ScratchAlloc>(mDeviceName);
			mDeviceName = NULL;
		}

		RenderWindow::destroy_internal();
	}

	void Win32Window::setFullscreen(bool fullScreen, UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsFullScreen != fullScreen || width != mWidth || height != mHeight)
		{
			mIsFullScreen = fullScreen;
			DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;

			if (mIsFullScreen)
			{
				dwStyle |= WS_POPUP;

				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = mColorDepth;
				displayDeviceMode.dmPelsWidth = width;
				displayDeviceMode.dmPelsHeight = height;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, 
						CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)					
					{
						// TODO LOG PORT - Log this somewhere
						//LogManager::getSingleton().logMessage(LML_NORMAL, "ChangeDisplaySettings with user display frequency failed");
						displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
					}
				}
				else
				{
					// try a few
					displayDeviceMode.dmDisplayFrequency = 100;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, 
						CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)		
					{
						displayDeviceMode.dmDisplayFrequency = 75;
						if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, 
							CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)		
						{
							displayDeviceMode.dmFields ^= DM_DISPLAYFREQUENCY;
						}
					}

				}
				// move window to 0,0 before display switch
				SetWindowPos(mHWnd, HWND_TOPMOST, 0, 0, mWidth, mHeight, SWP_NOACTIVATE);

				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)				
				{
					// TODO LOG PORT - Log this somewhere
					//LogManager::getSingleton().logMessage(LML_CRITICAL, "ChangeDisplaySettings failed");
				}

				// Get the nearest monitor to this window.
				HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

				// Get monitor info	
				MONITORINFO monitorInfo;

				memset(&monitorInfo, 0, sizeof(MONITORINFO));
				monitorInfo.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(hMonitor, &monitorInfo);

				mTop = monitorInfo.rcMonitor.top;
				mLeft = monitorInfo.rcMonitor.left;

				SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
				SetWindowPos(mHWnd, HWND_TOPMOST, mLeft, mTop, width, height,
					SWP_NOACTIVATE);
				mWidth = width;
				mHeight = height;
			}
			else
			{
				dwStyle |= WS_OVERLAPPEDWINDOW;

				// drop out of fullscreen
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);

				// calculate overall dimensions for requested client area
				unsigned int winWidth, winHeight;
				_adjustWindow(width, height, &winWidth, &winHeight);

				// deal with centreing when switching down to smaller resolution

				HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);
				MONITORINFO monitorInfo;
				memset(&monitorInfo, 0, sizeof(MONITORINFO));
				monitorInfo.cbSize = sizeof(MONITORINFO);
				GetMonitorInfo(hMonitor, &monitorInfo);

				LONG screenw = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
				LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;


				int left = screenw > int(winWidth) ? ((screenw - int(winWidth)) / 2) : 0;
				int top = screenh > int(winHeight) ? ((screenh - int(winHeight)) / 2) : 0;

				SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
				SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
					SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
				mWidth = width;
				mHeight = height;

				_windowMovedOrResized();
			}
		}
	}

	bool Win32Window::isActive(void) const
	{
		if (isFullScreen())
			return isVisible();

		return mActive && isVisible();
	}

	bool Win32Window::isVisible() const
	{
		return (mHWnd && !IsIconic(mHWnd));
	}

	bool Win32Window::isClosed() const
	{
		return mClosed;
	}

	void Win32Window::move(INT32 left, INT32 top)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && !mIsFullScreen)
		{
			mLeft = left;
			mTop = top;

			SetWindowPos(mHWnd, 0, left, top, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Win32Window::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && !mIsFullScreen)
		{
			mWidth = width;
			mHeight = height;

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			SetWindowPos(mHWnd, 0, 0, 0, width, height,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void Win32Window::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mIsExternalGLControl) {
			SwapBuffers(mHDC);
		}
	}

	void Win32Window::copyToMemory(const PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if ((dst.getLeft() < 0) || (dst.getRight() > mWidth) ||
			(dst.getTop() < 0) || (dst.getBottom() > mHeight) ||
			(dst.getFront() != 0) || (dst.getBack() != 1))
		{
			CM_EXCEPT(InvalidParametersException, "Invalid box.");
		}

		if (buffer == FB_AUTO)
		{
			buffer = mIsFullScreen? FB_FRONT : FB_BACK;
		}

		GLenum format = CamelotFramework::GLPixelUtil::getGLOriginFormat(dst.getFormat());
		GLenum type = CamelotFramework::GLPixelUtil::getGLOriginDataType(dst.getFormat());

		if ((format == GL_NONE) || (type == 0))
		{
			CM_EXCEPT(InvalidParametersException, "Unsupported format.");
		}

		// Must change the packing to ensure no overruns!
		glPixelStorei(GL_PACK_ALIGNMENT, 1);

		glReadBuffer((buffer == FB_FRONT)? GL_FRONT : GL_BACK);
		glReadPixels((GLint)dst.getLeft(), (GLint)dst.getTop(),
					 (GLsizei)dst.getWidth(), (GLsizei)dst.getHeight(),
					 format, type, dst.getData());

		// restore default alignment
		glPixelStorei(GL_PACK_ALIGNMENT, 4);

		//vertical flip
		{
			size_t rowSpan = dst.getWidth() * PixelUtil::getNumElemBytes(dst.getFormat());
			size_t height = dst.getHeight();
			UINT8 *tmpData = (UINT8*)cm_alloc<ScratchAlloc>((UINT32)(rowSpan * height));
			UINT8 *srcRow = (UINT8 *)dst.getData(), *tmpRow = tmpData + (height - 1) * rowSpan;

			while (tmpRow >= tmpData)
			{
				memcpy(tmpRow, srcRow, rowSpan);
				srcRow += rowSpan;
				tmpRow -= rowSpan;
			}
			memcpy(dst.getData(), tmpData, rowSpan * height);

			cm_free<ScratchAlloc>(tmpData);
		}
	}

	Vector2I Win32Window::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I Win32Window::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}	

	void Win32Window::getCustomAttribute( const String& name, void* pData ) const
	{
		if( name == "GLCONTEXT" ) {
			*static_cast<GLContext**>(pData) = mContext;
			return;
		} else if( name == "WINDOW" )
		{
			HWND *pHwnd = (HWND*)pData;
			*pHwnd = _getWindowHandle();
			return;
		} 
	}

	void Win32Window::setActive( bool state )
	{	
		THROW_IF_NOT_CORE_THREAD;

		if (mDeviceName != NULL && state == false)
		{
			HWND hActiveWindow = GetActiveWindow();
			char classNameSrc[_MAX_CLASS_NAME_ + 1];
			char classNameDst[_MAX_CLASS_NAME_ + 1];

			GetClassName(mHWnd, classNameSrc, _MAX_CLASS_NAME_);
			GetClassName(hActiveWindow, classNameDst, _MAX_CLASS_NAME_);

			if (strcmp(classNameDst, classNameSrc) == 0)
			{
				state = true;
			}						
		}
		
		mActive = state;

		if( mIsFullScreen )
		{
			if( state == false )
			{	//Restore Desktop
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
				ShowWindow(mHWnd, SW_SHOWMINNOACTIVE);
			}
			else
			{	//Restore App
				ShowWindow(mHWnd, SW_SHOWNORMAL);

				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = mColorDepth;
				displayDeviceMode.dmPelsWidth = mWidth;
				displayDeviceMode.dmPelsHeight = mHeight;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
				}
				ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL);
			}
		}
	}

	void Win32Window::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		mHidden = hidden;
		if (!mIsExternal)
		{
			if (hidden)
				ShowWindow(mHWnd, SW_HIDE);
			else
				ShowWindow(mHWnd, SW_SHOWNORMAL);
		}
	}

	void Win32Window::_windowMovedOrResized()
	{
		if (!mHWnd || IsIconic(mHWnd))
			return;

		RECT rc;
		// top and left represent outer window position
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;
		// width and height represent drawable area only
		GetClientRect(mHWnd, &rc);

		mWidth = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;

		RenderWindow::_windowMovedOrResized();
	}

	void Win32Window::_adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
		unsigned int* winWidth, unsigned int* winHeight)
	{
		// NB only call this for non full screen
		RECT rc;
		SetRect(&rc, 0, 0, clientWidth, clientHeight);
		AdjustWindowRect(&rc, WS_VISIBLE | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, false);
		*winWidth = rc.right - rc.left;
		*winHeight = rc.bottom - rc.top;

		// adjust to monitor
		HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

		// Get monitor info	
		MONITORINFO monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG maxW = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
		LONG maxH = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		if (*winWidth > (unsigned int)maxW)
			*winWidth = maxW;
		if (*winHeight > (unsigned int)maxH)
			*winHeight = maxH;

	}
}

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif

#include "BsWin32Window.h"
#include "BsInput.h"
#include "BsRenderAPI.h"
#include "BsCoreThread.h"
#include "BsException.h"
#include "BsWin32GLSupport.h"
#include "BsWin32Context.h"
#include "Win32/BsWin32Platform.h"
#include "BsWin32VideoModeInfo.h"
#include "BsGLPixelFormat.h"
#include "BsRenderWindowManager.h"

GLenum GLEWAPIENTRY wglewContextInit(BansheeEngine::GLSupport *glSupport);

namespace BansheeEngine 
{
	#define _MAX_CLASS_NAME_ 128

	Win32RenderWindowProperties::Win32RenderWindowProperties(const RENDER_WINDOW_DESC& desc)
		:RenderWindowProperties(desc)
	{ }

	Win32WindowCore::Win32WindowCore(const RENDER_WINDOW_DESC& desc, UINT32 windowId, Win32GLSupport& glsupport)
		: RenderWindowCore(desc, windowId), mProperties(desc), mSyncedProperties(desc), mGLSupport(glsupport), mContext(0), 
		mWindowedStyle(0), mWindowedStyleEx(0), mIsExternal(false), mIsExternalGLControl(false), mDisplayFrequency(0), mDeviceName(nullptr), mHWnd(0)
	{ }

	Win32WindowCore::~Win32WindowCore()
	{ 
		Win32RenderWindowProperties& props = mProperties;

		if (!mHWnd)
			return;

		if (!mIsExternal)
		{
			if (props.mIsFullScreen)
				ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);
			DestroyWindow(mHWnd);
		}
		else
		{
			// just release the DC
			ReleaseDC(mHWnd, mHDC);
		}

		props.mActive = false;
		mHDC = 0; // no release thanks to CS_OWNDC wndclass style
		mHWnd = 0;

		if (mDeviceName != NULL)
		{
			bs_free<ScratchAlloc>(mDeviceName);
			mDeviceName = NULL;
		}
	}

	void Win32WindowCore::initialize()
	{
		RenderWindowCore::initialize();

#ifdef BS_STATIC_LIB
		HINSTANCE hInst = GetModuleHandle(NULL);
#else
		HINSTANCE hInst = GetModuleHandle(MODULE_NAME.c_str());
#endif

		Win32RenderWindowProperties& props = mProperties;

		props.mIsFullScreen = mDesc.fullscreen;
		mIsChild = false;
		mDisplayFrequency = Math::roundToInt(mDesc.videoMode.getRefreshRate());
		props.mColorDepth = 32;
		HWND parent = 0;

		NameValuePairList::const_iterator opt;
		NameValuePairList::const_iterator end = mDesc.platformSpecific.end();

		if ((opt = mDesc.platformSpecific.find("externalWindowHandle")) != end)
		{
			mHWnd = (HWND)parseUnsignedInt(opt->second);
			if (mHWnd)
			{
				mIsExternal = true;
			}

			if ((opt = mDesc.platformSpecific.find("externalGLControl")) != end) {
				mIsExternalGLControl = parseBool(opt->second);
			}
		}

		HGLRC glrc = 0;
		if ((opt = mDesc.platformSpecific.find("externalGLContext")) != end)
		{
			glrc = (HGLRC)parseUnsignedLong(opt->second);
		}

		if ((opt = mDesc.platformSpecific.find("parentWindowHandle")) != end)
		{
			parent = (HWND)parseUnsignedInt(opt->second);
			mIsChild = true;
			props.mIsFullScreen = false;
		}

		HMONITOR hMonitor = NULL;
		const Win32VideoModeInfo& videoModeInfo = static_cast<const Win32VideoModeInfo&>(RenderAPICore::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs > 0)
		{
			UINT32 actualMonitorIdx = std::min(mDesc.videoMode.getOutputIdx(), numOutputs - 1);
			const Win32VideoOutputInfo& outputInfo = static_cast<const Win32VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));
			hMonitor = outputInfo.getMonitorHandle();
		}

		if (!props.mIsFullScreen)
		{
			// Make sure we don't exceed desktop color depth
			if ((int)props.mColorDepth > GetDeviceCaps(GetDC(0), BITSPIXEL))
				props.mColorDepth = GetDeviceCaps(GetDC(0), BITSPIXEL);
		}

		mWindowedStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		mWindowedStyleEx = 0;

		if (!props.mIsFullScreen)
		{
			if (parent)
			{
				if (mDesc.toolWindow)
					mWindowedStyleEx = WS_EX_TOOLWINDOW;
				else
					mWindowedStyle |= WS_CHILD;
			}

			if (!parent || mDesc.toolWindow)
			{
				if (mDesc.border == WindowBorder::None)
					mWindowedStyle |= WS_POPUP;
				else if (mDesc.border == WindowBorder::Fixed)
					mWindowedStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
					WS_SYSMENU | WS_MINIMIZEBOX;
				else
					mWindowedStyle |= WS_OVERLAPPEDWINDOW;
			}
		}

		if (!mIsExternal)
		{
			RECT rc;

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
			MONITORINFOEX monitorInfoEx;
			memset(&monitorInfoEx, 0, sizeof(MONITORINFOEX));
			monitorInfoEx.cbSize = sizeof(MONITORINFOEX);

			GetMonitorInfo(hMonitor, &monitorInfoEx);

			size_t devNameLen = strlen(monitorInfoEx.szDevice);
			mDeviceName = (char*)bs_alloc<ScratchAlloc>((UINT32)(devNameLen + 1));

			strcpy_s(mDeviceName, devNameLen + 1, monitorInfoEx.szDevice);

			UINT32 left = mDesc.left;
			UINT32 top = mDesc.top;

			// No specified top left -> Center the window in the middle of the monitor
			if (left == -1 || top == -1)
			{
				int screenw = monitorInfoEx.rcWork.right - monitorInfoEx.rcWork.left;
				int screenh = monitorInfoEx.rcWork.bottom - monitorInfoEx.rcWork.top;

				unsigned int winWidth, winHeight;
				getAdjustedWindowSize(mDesc.videoMode.getWidth(), mDesc.videoMode.getHeight(), &winWidth, &winHeight);

				// clamp window dimensions to screen size
				int outerw = (int(winWidth) < screenw) ? int(winWidth) : screenw;
				int outerh = (int(winHeight) < screenh) ? int(winHeight) : screenh;

				if (left == -1)
					left = monitorInfoEx.rcWork.left + (screenw - outerw) / 2;
				else if (hMonitor != NULL)
					left += monitorInfoEx.rcWork.left;

				if (top == -1)
					top = monitorInfoEx.rcWork.top + (screenh - outerh) / 2;
				else if (hMonitor != NULL)
					top += monitorInfoEx.rcWork.top;
			}
			else if (hMonitor != NULL)
			{
				left += monitorInfoEx.rcWork.left;
				top += monitorInfoEx.rcWork.top;
			}

			props.mWidth = mDesc.videoMode.getWidth();
			props.mHeight = mDesc.videoMode.getHeight();
			props.mTop = top;
			props.mLeft = left;

			DWORD dwStyle = 0;
			DWORD dwStyleEx = 0;
			if (props.mIsFullScreen)
			{
				dwStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_POPUP;
				props.mTop = monitorInfoEx.rcMonitor.top;
				props.mLeft = monitorInfoEx.rcMonitor.left;
			}
			else
			{
				dwStyle = mWindowedStyle;
				dwStyleEx = mWindowedStyleEx;

				int screenw = GetSystemMetrics(SM_CXSCREEN);
				int screenh = GetSystemMetrics(SM_CYSCREEN);

				if (!mDesc.outerDimensions)
				{
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, props.mWidth, props.mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					props.mWidth = rc.right - rc.left;
					props.mHeight = rc.bottom - rc.top;

					// Clamp window rect to the nearest display monitor.
					if (props.mLeft < monitorInfoEx.rcWork.left)
						props.mLeft = monitorInfoEx.rcWork.left;

					if (props.mTop < monitorInfoEx.rcWork.top)
						props.mTop = monitorInfoEx.rcWork.top;

					if ((int)props.mWidth > monitorInfoEx.rcWork.right - props.mLeft)
						props.mWidth = monitorInfoEx.rcWork.right - props.mLeft;

					if ((int)props.mHeight > monitorInfoEx.rcWork.bottom - props.mTop)
						props.mHeight = monitorInfoEx.rcWork.bottom - props.mTop;
				}
			}

			// register class and create window
			WNDCLASS wc = { CS_OWNDC, Win32Platform::_win32WndProc, 0, 0, hInst,
				LoadIcon(NULL, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), NULL, "GLWindow" };
			RegisterClass(&wc);

			if (props.mIsFullScreen)
			{
				DEVMODE displayDeviceMode;

				memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
				displayDeviceMode.dmSize = sizeof(DEVMODE);
				displayDeviceMode.dmBitsPerPel = props.mColorDepth;
				displayDeviceMode.dmPelsWidth = props.mWidth;
				displayDeviceMode.dmPelsHeight = props.mHeight;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;

					if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN | CDS_TEST, NULL) != DISP_CHANGE_SUCCESSFUL)
					{
						BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings with user display frequency failed.");
					}
				}

				if (ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
				{
					BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings failed.");
				}
			}

			// Pass pointer to self as WM_CREATE parameter
			mHWnd = CreateWindowEx(dwStyleEx, "GLWindow", mDesc.title.c_str(),
				dwStyle, props.mLeft, props.mTop, props.mWidth, props.mHeight, parent, 0, hInst, this);
		}

		RECT rc;

		GetWindowRect(mHWnd, &rc);
		props.mTop = rc.top;
		props.mLeft = rc.left;

		GetClientRect(mHWnd, &rc);
		props.mWidth = rc.right;
		props.mHeight = rc.bottom;

		mHDC = GetDC(mHWnd);

		if (!mIsExternalGLControl)
		{
			int testMultisample = props.mMultisampleCount;
			bool testHwGamma = mDesc.gamma;
			bool formatOk = mGLSupport.selectPixelFormat(mHDC, props.mColorDepth, testMultisample, testHwGamma);
			if (!formatOk)
			{
				if (props.mMultisampleCount > 0)
				{
					// Try without multisampling
					testMultisample = 0;
					formatOk = mGLSupport.selectPixelFormat(mHDC, props.mColorDepth, testMultisample, testHwGamma);
				}

				if (!formatOk && mDesc.gamma)
				{
					// Try without sRGB
					testHwGamma = false;
					testMultisample = props.mMultisampleCount;
					formatOk = mGLSupport.selectPixelFormat(mHDC, props.mColorDepth, testMultisample, testHwGamma);
				}

				if (!formatOk && mDesc.gamma && (props.mMultisampleCount > 0))
				{
					// Try without both
					testHwGamma = false;
					testMultisample = 0;
					formatOk = mGLSupport.selectPixelFormat(mHDC, props.mColorDepth, testMultisample, testHwGamma);
				}

				if (!formatOk)
					BS_EXCEPT(RenderingAPIException, "Failed selecting pixel format.");

			}

			// Record what gamma option we used in the end
			// this will control enabling of sRGB state flags when used
			props.mHwGamma = testHwGamma;
			props.mMultisampleCount = testMultisample;
		}

		props.mActive = true;
		mContext = mGLSupport.createContext(mHDC, glrc);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties = props;
		}

		RenderWindowManager::instance().notifySyncDataDirty(this);
	}

	void Win32WindowCore::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const Win32VideoModeInfo& videoModeInfo = static_cast<const Win32VideoModeInfo&>(RenderAPICore::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		Win32RenderWindowProperties& props = mProperties;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const Win32VideoOutputInfo& outputInfo = static_cast<const Win32VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		bool oldFullscreen = props.mIsFullScreen;

		mDisplayFrequency = Math::roundToInt(refreshRate);
		props.mIsFullScreen = true;

		DEVMODE displayDeviceMode;

		memset(&displayDeviceMode, 0, sizeof(displayDeviceMode));
		displayDeviceMode.dmSize = sizeof(DEVMODE);
		displayDeviceMode.dmBitsPerPel = props.mColorDepth;
		displayDeviceMode.dmPelsWidth = width;
		displayDeviceMode.dmPelsHeight = height;
		displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
		displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

		HMONITOR hMonitor = outputInfo.getMonitorHandle();
		MONITORINFOEX monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFOEX));
		monitorInfo.cbSize = sizeof(MONITORINFOEX);
		GetMonitorInfo(hMonitor, &monitorInfo);

		if (ChangeDisplaySettingsEx(monitorInfo.szDevice, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL) != DISP_CHANGE_SUCCESSFUL)
		{
			BS_EXCEPT(RenderingAPIException, "ChangeDisplaySettings failed");
		}

		props.mTop = monitorInfo.rcMonitor.top;
		props.mLeft = monitorInfo.rcMonitor.left;
		props.mWidth = width;
		props.mHeight = height;

		SetWindowPos(mHWnd, HWND_TOP, props.mLeft, props.mTop, width, height, SWP_NOACTIVATE);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties.mTop = props.mTop;
			mSyncedProperties.mLeft = props.mLeft;
			mSyncedProperties.mWidth = props.mWidth;
			mSyncedProperties.mHeight = props.mHeight;
		}

		RenderWindowManager::instance().notifySyncDataDirty(this);
		RenderWindowManager::instance().notifyMovedOrResized(this);
	}

	void Win32WindowCore::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		setFullscreen(mode.getWidth(), mode.getHeight(), mode.getRefreshRate(), mode.getOutputIdx());
	}

	void Win32WindowCore::setWindowed(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		Win32RenderWindowProperties& props = mProperties;

		if (!props.mIsFullScreen)
			return;

		props.mIsFullScreen = false;
		props.mWidth = width;
		props.mHeight = height;

		// Drop out of fullscreen
		ChangeDisplaySettingsEx(mDeviceName, NULL, NULL, 0, NULL);

		// Calculate overall dimensions for requested client area
		UINT32 winWidth, winHeight;
		getAdjustedWindowSize(props.mWidth, props.mHeight, &winWidth, &winHeight);

		// Deal with centering when switching down to smaller resolution
		HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		INT32 left = screenw > INT32(winWidth) ? ((screenw - INT32(winWidth)) / 2) : 0;
		INT32 top = screenh > INT32(winHeight) ? ((screenh - INT32(winHeight)) / 2) : 0;

		SetWindowLong(mHWnd, GWL_STYLE, mWindowedStyle);
		SetWindowLong(mHWnd, GWL_EXSTYLE, mWindowedStyleEx);
		SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
			SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties.mWidth = props.mWidth;
			mSyncedProperties.mHeight = props.mHeight;
		}

		RenderWindowManager::instance().notifySyncDataDirty(this);
		_windowMovedOrResized();
	}

	void Win32WindowCore::move(INT32 left, INT32 top)
	{
		THROW_IF_NOT_CORE_THREAD;

		Win32RenderWindowProperties& props = mProperties;
		if (mHWnd && !props.mIsFullScreen)
		{
			props.mLeft = left;
			props.mTop = top;

			SetWindowPos(mHWnd, 0, left, top, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mTop = props.mTop;
				mSyncedProperties.mLeft = props.mLeft;
			}

			RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void Win32WindowCore::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		Win32RenderWindowProperties& props = mProperties;
		if (mHWnd && !props.mIsFullScreen)
		{
			props.mWidth = width;
			props.mHeight = height;

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			SetWindowPos(mHWnd, 0, 0, 0, width, height,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mWidth = props.mWidth;
				mSyncedProperties.mHeight = props.mHeight;
			}

			RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void Win32WindowCore::minimize()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd)
			SendMessage(mHWnd, WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}

	void Win32WindowCore::maximize()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd)
			SendMessage(mHWnd, WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}

	void Win32WindowCore::restore()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd)
			SendMessage(mHWnd, WM_SYSCOMMAND, SC_RESTORE, 0);
	}

	void Win32WindowCore::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mIsExternalGLControl) {
			SwapBuffers(mHDC);
		}
	}

	void Win32WindowCore::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if ((dst.getLeft() < 0) || (dst.getRight() > getProperties().getWidth()) ||
			(dst.getTop() < 0) || (dst.getBottom() > getProperties().getHeight()) ||
			(dst.getFront() != 0) || (dst.getBack() != 1))
		{
			BS_EXCEPT(InvalidParametersException, "Invalid box.");
		}

		if (buffer == FB_AUTO)
		{
			buffer = mProperties.isFullScreen() ? FB_FRONT : FB_BACK;
		}

		GLenum format = BansheeEngine::GLPixelUtil::getGLOriginFormat(dst.getFormat());
		GLenum type = BansheeEngine::GLPixelUtil::getGLOriginDataType(dst.getFormat());

		if ((format == GL_NONE) || (type == 0))
		{
			BS_EXCEPT(InvalidParametersException, "Unsupported format.");
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
			UINT8 *tmpData = (UINT8*)bs_alloc<ScratchAlloc>((UINT32)(rowSpan * height));
			UINT8 *srcRow = (UINT8 *)dst.getData(), *tmpRow = tmpData + (height - 1) * rowSpan;

			while (tmpRow >= tmpData)
			{
				memcpy(tmpRow, srcRow, rowSpan);
				srcRow += rowSpan;
				tmpRow -= rowSpan;
			}
			memcpy(dst.getData(), tmpData, rowSpan * height);

			bs_free<ScratchAlloc>(tmpData);
		}
	}

	void Win32WindowCore::getCustomAttribute(const String& name, void* pData) const
	{
		if(name == "GLCONTEXT") 
		{
			SPtr<GLContext>* contextPtr = static_cast<SPtr<GLContext>*>(pData);
			*contextPtr = mContext;
			return;
		} 
		else if(name == "WINDOW")
		{
			UINT64 *pHwnd = (UINT64*)pData;
			*pHwnd = (UINT64)mHWnd;
			return;
		} 
	}

	void Win32WindowCore::setActive(bool state)
	{	
		THROW_IF_NOT_CORE_THREAD;

		Win32RenderWindowProperties& props = mProperties;
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
		
		props.mActive = state;

		if(props.mIsFullScreen)
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
				displayDeviceMode.dmBitsPerPel = props.mColorDepth;
				displayDeviceMode.dmPelsWidth = props.mWidth;
				displayDeviceMode.dmPelsHeight = props.mHeight;
				displayDeviceMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
				if (mDisplayFrequency)
				{
					displayDeviceMode.dmDisplayFrequency = mDisplayFrequency;
					displayDeviceMode.dmFields |= DM_DISPLAYFREQUENCY;
				}
				ChangeDisplaySettingsEx(mDeviceName, &displayDeviceMode, NULL, CDS_FULLSCREEN, NULL);
			}
		}

		RenderWindowCore::setActive(state);
	}

	void Win32WindowCore::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		Win32RenderWindowProperties& props = mProperties;
		props.mHidden = hidden;
		if (!mIsExternal)
		{
			if (hidden)
				ShowWindow(mHWnd, SW_HIDE);
			else
				ShowWindow(mHWnd, SW_SHOWNORMAL);
		}

		RenderWindowCore::setHidden(hidden);
	}

	void Win32WindowCore::_windowMovedOrResized()
	{
		Win32RenderWindowProperties& props = mProperties;

		if (!mHWnd || IsIconic(mHWnd))
			return;

		RECT rc;
		GetWindowRect(mHWnd, &rc);
		props.mTop = rc.top;
		props.mLeft = rc.left;

		GetClientRect(mHWnd, &rc);
		props.mWidth = rc.right - rc.left;
		props.mHeight = rc.bottom - rc.top;

		RenderWindowCore::_windowMovedOrResized();
	}

	void Win32WindowCore::getAdjustedWindowSize(UINT32 clientWidth, UINT32 clientHeight, UINT32* winWidth, UINT32* winHeight)
	{
		Win32RenderWindowProperties& props = mProperties;

		RECT rc;
		SetRect(&rc, 0, 0, clientWidth, clientHeight);
		AdjustWindowRectEx(&rc, mWindowedStyle, false, mWindowedStyleEx);
		*winWidth = rc.right - rc.left;
		*winHeight = rc.bottom - rc.top;

		// Adjust to monitor
		HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);

		// Get monitor info	
		MONITORINFO monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG maxW = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
		LONG maxH = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		if (*winWidth > (UINT32)maxW)
			*winWidth = maxW;

		if (*winHeight > (UINT32)maxH)
			*winHeight = maxH;
	}

	void Win32WindowCore::syncProperties()
	{
		ScopedSpinLock lock(mLock);
		mProperties = mSyncedProperties;
	}

	Win32Window::Win32Window(const RENDER_WINDOW_DESC& desc, UINT32 windowId, Win32GLSupport &glsupport)
		:RenderWindow(desc, windowId), mGLSupport(glsupport), mProperties(desc)
	{

	}

	void Win32Window::getCustomAttribute(const String& name, void* pData) const
	{
		if (name == "WINDOW")
		{
			UINT64 *pHwnd = (UINT64*)pData;
			*pHwnd = (UINT64)getHWnd();
			return;
		}
	}

	Vector2I Win32Window::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I Win32Window::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	SPtr<Win32WindowCore> Win32Window::getCore() const
	{
		return std::static_pointer_cast<Win32WindowCore>(mCoreSpecific);
	}

	void Win32Window::syncProperties()
	{
		ScopedSpinLock lock(getCore()->mLock);
		mProperties = getCore()->mSyncedProperties;
	}

	HWND Win32Window::getHWnd() const
	{
		// HACK: I'm accessing core method from sim thread, which means an invalid handle
		// could be returned here if requested too soon after initialization.
		return getCore()->_getHWnd();
	}
}

#include "CmD3D9RenderWindow.h"
#include "CmInput.h"
#include "CmCoreThread.h"
#include "CmViewport.h"
#include "CmException.h"
#include "CmD3D9RenderSystem.h"
#include "CmRenderSystem.h"
#include "CmBitwise.h"
#include "Win32/CmPlatformWndProc.h"
#include "CmD3D9VideoModeInfo.h"
#include "CmD3D9DeviceManager.h"

namespace BansheeEngine
{
	D3D9RenderWindow::D3D9RenderWindow(const RENDER_WINDOW_DESC& desc, HINSTANCE instance)
		: RenderWindow(desc), mInstance(instance), mIsDepthBuffered(true), mIsChild(false),
		mStyle(0), mWindowedStyle(0), mWindowedStyleEx(0)
	{
		mDevice = NULL;
		mIsFullScreen = false;		
		mIsExternal = false;
		mHWnd = 0;
		mActive = false;		
		mClosed = false;
		mDisplayFrequency = 0;
		mDeviceValid = false;
	}

	D3D9RenderWindow::~D3D9RenderWindow()
	{ }

	void D3D9RenderWindow::initialize_internal()
	{
		HINSTANCE hInst = mInstance;

		mMultisampleType = D3DMULTISAMPLE_NONE;
		mMultisampleQuality = 0;
		mMultisampleCount = mDesc.multisampleCount;
		mVSync = mDesc.vsync;
		mVSyncInterval = mDesc.vsyncInterval;
		mDisplayFrequency = Math::roundToInt(mDesc.videoMode.getRefreshRate());

		HWND parentHWnd = 0;
		HWND externalHandle = 0;

		NameValuePairList::const_iterator opt;
		opt = mDesc.platformSpecific.find("parentWindowHandle");
		if(opt != mDesc.platformSpecific.end())
			parentHWnd = (HWND)parseUnsignedInt(opt->second);

		opt = mDesc.platformSpecific.find("externalWindowHandle");
		if(opt != mDesc.platformSpecific.end())
			externalHandle = (HWND)parseUnsignedInt(opt->second);

		mIsChild = parentHWnd != 0;

		mWindowedStyle = WS_VISIBLE | WS_CLIPCHILDREN;
		mWindowedStyleEx = 0;

		if (!mDesc.fullscreen || mIsChild)
		{
			if (parentHWnd)
			{
				if (mDesc.toolWindow)
					mWindowedStyleEx = WS_EX_TOOLWINDOW;
				else
					mWindowedStyle |= WS_CHILD;
			}

			if (!parentHWnd || mDesc.toolWindow)
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

		if (!externalHandle)
		{
			MONITORINFO monitorInfo;
			RECT rc;

			HMONITOR hMonitor = NULL;
			const D3D9VideoModeInfo& videoModeInfo = static_cast<const D3D9VideoModeInfo&>(RenderSystem::instance().getVideoModeInfo());
			UINT32 numOutputs = videoModeInfo.getNumOutputs();
			if (numOutputs > 0)
			{
				UINT32 actualMonitorIdx = std::min(mDesc.videoMode.getOutputIdx(), numOutputs - 1);
				const D3D9VideoOutputInfo& outputInfo = static_cast<const D3D9VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));
				hMonitor = outputInfo.getMonitorHandle();
			}

			// If we didn't specified the adapter index, or if it didn't find it
			if (hMonitor == NULL)
			{
				POINT windowAnchorPoint;

				// Fill in anchor point.
				windowAnchorPoint.x = mDesc.left;
				windowAnchorPoint.y = mDesc.top;

				// Get the nearest monitor to this window.
				hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
			}

			// Get the target monitor info		
			memset(&monitorInfo, 0, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);

			unsigned int winWidth, winHeight;
			winWidth = mDesc.videoMode.getWidth();
			winHeight = mDesc.videoMode.getHeight();

			UINT32 left = mDesc.left;
			UINT32 top = mDesc.top;

			// No specified top left -> Center the window in the middle of the monitor
			if (left == -1 || top == -1)
			{				
				int screenw = monitorInfo.rcWork.right  - monitorInfo.rcWork.left;
				int screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

				// clamp window dimensions to screen size
				int outerw = (int(winWidth) < screenw)? int(winWidth) : screenw;
				int outerh = (int(winHeight) < screenh)? int(winHeight) : screenh;

				if (left == -1)
					left = monitorInfo.rcWork.left + (screenw - outerw) / 2;
				else if (hMonitor != NULL)
					left += monitorInfo.rcWork.left;

				if (top == -1)
					top = monitorInfo.rcWork.top + (screenh - outerh) / 2;
				else if (hMonitor != NULL)
					top += monitorInfo.rcWork.top;
			}
			else if (hMonitor != NULL)
			{
				left += monitorInfo.rcWork.left;
				top += monitorInfo.rcWork.top;
			}

			mWidth = mDesc.videoMode.getWidth();
			mHeight =  mDesc.videoMode.getHeight();
			mTop = top;
			mLeft = left;

			DWORD dwStyle = 0;
			DWORD dwStyleEx = 0;
			if (mDesc.fullscreen && !mIsChild)
			{
				dwStyle = WS_VISIBLE | WS_CLIPCHILDREN | WS_POPUP;
				mTop = monitorInfo.rcMonitor.top;
				mLeft = monitorInfo.rcMonitor.left;		
			}
			else
			{
				dwStyle = mWindowedStyle;
				dwStyleEx = mWindowedStyleEx;

				_adjustWindow(mDesc.videoMode.getWidth(), mDesc.videoMode.getHeight(), dwStyle, &winWidth, &winHeight);

				if (!mDesc.outerDimensions)
				{
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, mWidth, mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					mWidth = rc.right - rc.left;
					mHeight = rc.bottom - rc.top;

					// Clamp window rect to the nearest display monitor.
					if (mLeft < monitorInfo.rcWork.left)
						mLeft = monitorInfo.rcWork.left;		

					if (mTop < monitorInfo.rcWork.top)					
						mTop = monitorInfo.rcWork.top;					

					if (static_cast<int>(winWidth) > monitorInfo.rcWork.right - mLeft)					
						winWidth = monitorInfo.rcWork.right - mLeft;	

					if (static_cast<int>(winHeight) > monitorInfo.rcWork.bottom - mTop)					
						winHeight = monitorInfo.rcWork.bottom - mTop;										
				}
			}


			// Register the window class
			// NB allow 4 bytes of window data for D3D9RenderWindow pointer
			WNDCLASS wc = { 0, PlatformWndProc::_win32WndProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "D3D9Wnd" };
			RegisterClass(&wc);

			// Create our main window
			// Pass pointer to self
			mIsExternal = false;
			mHWnd = CreateWindowEx(dwStyleEx, "D3D9Wnd", mDesc.title.c_str(), dwStyle,
				mLeft, mTop, winWidth, winHeight, parentHWnd, 0, hInst, this);
			mStyle = dwStyle;
		}
		else
		{
			mHWnd = externalHandle;
			mIsExternal = true;
		}

		RECT rc;
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;

		GetClientRect(mHWnd, &rc);
		mWidth = rc.right;
		mHeight = rc.bottom;

		mName = mDesc.title;
		mIsDepthBuffered = mDesc.depthBuffer;
		mIsFullScreen = mDesc.fullscreen && !mIsChild;
		mColorDepth = 32;

		mActive = true;
		mClosed = false;

		D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(RenderSystem::instancePtr());
		rs->registerWindow(*this);

		RenderWindow::initialize_internal();
	}

	void D3D9RenderWindow::destroy_internal()
	{
		if (mDevice != NULL)
		{
			mDevice->detachRenderWindow(this);
			mDevice = NULL;
		}

		if (mHWnd && !mIsExternal)
		{
			DestroyWindow(mHWnd);
		}

		mHWnd = 0;
		mActive = false;
		mClosed = true;

		RenderWindow::destroy_internal();
	}

	void D3D9RenderWindow::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const D3D9VideoModeInfo& videoModeInfo = static_cast<const D3D9VideoModeInfo&>(RenderSystem::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const D3D9VideoOutputInfo& outputInfo = static_cast<const D3D9VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		bool oldFullscreen = mIsFullScreen;

		mWidth = width;
		mHeight = height;
		mDisplayFrequency = Math::roundToInt(refreshRate);

		mIsFullScreen = true;

		HMONITOR hMonitor = outputInfo.getMonitorHandle();
		MONITORINFO monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		mTop = monitorInfo.rcMonitor.top;
		mLeft = monitorInfo.rcMonitor.left;

		// Invalidate device, which resets it
		mDevice->invalidate(this);
		mDevice->acquire();
	}

	void D3D9RenderWindow::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		setFullscreen(mode.getWidth(), mode.getHeight(), mode.getRefreshRate(), mode.getOutputIdx());
	}

	void D3D9RenderWindow::setWindowed()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mIsFullScreen)
			return;

		mIsFullScreen = false;

		mStyle = mWindowedStyle;

		unsigned int winWidth, winHeight;
		_adjustWindow(mWidth, mHeight, mStyle, &winWidth, &winHeight);

		// Deal with centering when switching down to smaller resolution
		HMONITOR hMonitor = MonitorFromWindow(mHWnd, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		int left = screenw > int(winWidth) ? ((screenw - int(winWidth)) / 2) : 0;
		int top = screenh > int(winHeight) ? ((screenh - int(winHeight)) / 2) : 0;

		SetWindowLong(mHWnd, GWL_STYLE, mStyle);
		SetWindowLong(mHWnd, GWL_EXSTYLE, mWindowedStyleEx);
		SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
			SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

		mDevice->invalidate(this);
		mDevice->acquire();
	}

	void D3D9RenderWindow::setHidden(bool hidden)
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

	bool D3D9RenderWindow::isActive() const
	{
		if (isFullScreen())
			return isVisible();

		return mActive && isVisible();
	}

	bool D3D9RenderWindow::isVisible() const
	{
		return (mHWnd && !IsIconic(mHWnd));
	}

	void D3D9RenderWindow::move(INT32 top, INT32 left)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && !mIsFullScreen)
		{
			mLeft = left;
			mTop = top;

			SetWindowPos(mHWnd, 0, top, left, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void D3D9RenderWindow::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && !mIsFullScreen)
		{
			mWidth = width;
			mHeight = height;

			unsigned int winWidth, winHeight;
			_adjustWindow(width, height, mStyle, &winWidth, &winHeight);

			SetWindowPos(mHWnd, 0, 0, 0, winWidth, winHeight,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void D3D9RenderWindow::getCustomAttribute( const String& name, void* pData ) const
	{
		// Valid attributes and their equivalent native functions:
		// D3DDEVICE			: getD3DDevice
		// WINDOW				: getWindowHandle

		if( name == "D3DDEVICE" )
		{
			IDirect3DDevice9* *pDev = (IDirect3DDevice9**)pData;
			*pDev = _getD3D9Device();
			return;
		}		
		else if( name == "WINDOW" )
		{
			HWND *pHwnd = (HWND*)pData;
			*pHwnd = _getWindowHandle();
			return;
		}
		else if( name == "isTexture" )
		{
			bool *b = reinterpret_cast< bool * >( pData );
			*b = false;

			return;
		}
		else if( name == "D3DZBUFFER" )
		{
			IDirect3DSurface9* *pSurf = (IDirect3DSurface9**)pData;
			*pSurf = mDevice->getDepthBuffer(this);
			return;
		}
		else if( name == "DDBACKBUFFER" )
		{
			IDirect3DSurface9* *pSurf = (IDirect3DSurface9**)pData;
			*pSurf = mDevice->getBackBuffer(this);
			return;
		}
		else if( name == "DDFRONTBUFFER" )
		{
			IDirect3DSurface9* *pSurf = (IDirect3DSurface9**)pData;
			*pSurf = mDevice->getBackBuffer(this);
			return;
		}
	}

	void D3D9RenderWindow::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mDeviceValid)
			mDevice->present(this);		
	}

	Vector2I D3D9RenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I D3D9RenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	void D3D9RenderWindow::copyToMemory(const PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		mDevice->copyContentsToMemory(this, dst, buffer);
	}

	void D3D9RenderWindow::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mHWnd || IsIconic(mHWnd))
			return;
	
		updateWindowRect();

		RenderWindow::_windowMovedOrResized();
	}

	/************************************************************************/
	/* 						D3D9 IMPLEMENTATION SPECIFIC                    */
	/************************************************************************/

	void D3D9RenderWindow::_adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
		DWORD style, unsigned int* winWidth, unsigned int* winHeight)
	{
		// NB only call this for non full screen
		RECT rc;
		SetRect(&rc, 0, 0, clientWidth, clientHeight);
		AdjustWindowRect(&rc, style, false);
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
	
	void D3D9RenderWindow::_buildPresentParameters(D3DPRESENT_PARAMETERS* presentParams) const
	{			
		IDirect3D9* pD3D = D3D9RenderSystem::getDirect3D9();
		D3DDEVTYPE devType = D3DDEVTYPE_HAL;

		if (mDevice != NULL)		
			devType = mDevice->getDeviceType();		
	
		ZeroMemory( presentParams, sizeof(D3DPRESENT_PARAMETERS) );
		presentParams->Windowed = !mIsFullScreen;
		presentParams->SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentParams->BackBufferCount = 1;
		presentParams->EnableAutoDepthStencil = mIsDepthBuffered;
		presentParams->hDeviceWindow = mHWnd;
		presentParams->BackBufferWidth = mWidth;
		presentParams->BackBufferHeight	= mHeight;
		presentParams->FullScreen_RefreshRateInHz = mIsFullScreen ? mDisplayFrequency : 0;
		
		if (presentParams->BackBufferWidth == 0)		
			presentParams->BackBufferWidth = 1;					

		if (presentParams->BackBufferHeight == 0)	
			presentParams->BackBufferHeight = 1;					

		if (mVSync)
		{
			if (mIsFullScreen)
			{
				switch(mVSyncInterval)
				{
				case 1:
				default:
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
					break;
				case 2:
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_TWO;
					break;
				case 3:
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_THREE;
					break;
				case 4:
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_FOUR;
					break;
				};

				D3DCAPS9 caps;
				pD3D->GetDeviceCaps(mDevice->getAdapterNumber(), devType, &caps);
				if (!(caps.PresentationIntervals & presentParams->PresentationInterval))
				{
					presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
				}

			}
			else
			{
				presentParams->PresentationInterval = D3DPRESENT_INTERVAL_ONE;
			}
		}
		else
		{
			presentParams->PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		}

		presentParams->BackBufferFormat = D3DFMT_X8R8G8B8;

		if (FAILED(pD3D->CheckDeviceFormat(mDevice->getAdapterNumber(),
			devType, presentParams->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
			D3DRTYPE_SURFACE, D3DFMT_D24S8)))
		{
			if (FAILED(pD3D->CheckDeviceFormat(mDevice->getAdapterNumber(),
				devType, presentParams->BackBufferFormat, D3DUSAGE_DEPTHSTENCIL,
				D3DRTYPE_SURFACE, D3DFMT_D32)))
			{
				presentParams->AutoDepthStencilFormat = D3DFMT_D16;
			}
			else
			{
				presentParams->AutoDepthStencilFormat = D3DFMT_D32;
			}
		}
		else
		{
			if (SUCCEEDED(pD3D->CheckDepthStencilMatch(mDevice->getAdapterNumber(), devType,
				presentParams->BackBufferFormat, presentParams->BackBufferFormat, D3DFMT_D24S8)))
			{
				presentParams->AutoDepthStencilFormat = D3DFMT_D24S8;
			}
			else
			{
				presentParams->AutoDepthStencilFormat = D3DFMT_D24X8;
			}
		}

		D3D9RenderSystem* rs = static_cast<D3D9RenderSystem*>(BansheeEngine::RenderSystem::instancePtr());

		D3DMULTISAMPLE_TYPE multisampleType;
		DWORD multisampleQuality;

		rs->determineMultisampleSettings(mDevice->getD3D9Device(), mMultisampleCount, mMultisampleHint, 
			presentParams->BackBufferFormat, mIsFullScreen, &multisampleType, &multisampleQuality);

		presentParams->MultiSampleType = multisampleType;
		presentParams->MultiSampleQuality = (multisampleQuality == 0) ? 0 : multisampleQuality;
	}

	IDirect3DDevice9* D3D9RenderWindow::_getD3D9Device() const
	{
		return mDevice->getD3D9Device();
	}

	IDirect3DSurface9* D3D9RenderWindow::_getRenderSurface() const 
	{
		return mDevice->getBackBuffer(this);
	}

	D3D9Device* D3D9RenderWindow::_getDevice() const
	{
		return mDevice;
	}

	void D3D9RenderWindow::_setDevice(D3D9Device* device)
	{
		mDevice = device;
		mDeviceValid = false;
	}

	bool D3D9RenderWindow::_isDepthBuffered() const
	{
		return mIsDepthBuffered;
	}

	void D3D9RenderWindow::updateWindowRect()
	{
		RECT rc;
		BOOL result;

		// Update top left parameters
		result = GetWindowRect(mHWnd, &rc);
		if (result == FALSE)
		{
			mTop = 0;
			mLeft = 0;
			mWidth = 0;
			mHeight = 0;
			return;
		}
		
		mTop = rc.top;
		mLeft = rc.left;

		// Width and height represent drawable area only
		result = GetClientRect(mHWnd, &rc);
		if (result == FALSE)
		{
			mTop = 0;
			mLeft = 0;
			mWidth = 0;
			mHeight = 0;
			return;
		}

		mWidth  = rc.right - rc.left;
		mHeight = rc.bottom - rc.top;	
	}

	bool D3D9RenderWindow::_validateDevice()
	{
		mDeviceValid = mDevice->validate(this);
		return mDeviceValid;
	}
}

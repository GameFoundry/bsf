//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9RenderWindow.h"
#include "BsInput.h"
#include "BsCoreThread.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsD3D9RenderAPI.h"
#include "BsRenderAPI.h"
#include "BsBitwise.h"
#include "Win32/BsWin32Platform.h"
#include "BsD3D9VideoModeInfo.h"
#include "BsD3D9DeviceManager.h"
#include "BsRenderWindowManager.h"
#include "Win32/BsWin32Window.h"

namespace BansheeEngine
{
	D3D9RenderWindowProperties::D3D9RenderWindowProperties(const RENDER_WINDOW_DESC& desc)
		:RenderWindowProperties(desc)
	{

	}

	D3D9RenderWindowCore::D3D9RenderWindowCore(const RENDER_WINDOW_DESC& desc, UINT32 windowId, HINSTANCE instance)
		: RenderWindowCore(desc, windowId), mInstance(instance), mProperties(desc), mSyncedProperties(desc), 
		mIsDepthBuffered(true), mIsChild(false), mWindow(nullptr),mDevice(nullptr), mDisplayFrequency(0), 
		mDeviceValid(false), mShowOnSwap(false)
	{ }

	D3D9RenderWindowCore::~D3D9RenderWindowCore()
	{ 
		if (mDevice != nullptr)
		{
			mDevice->detachRenderWindow(this);
			mDevice = nullptr;
		}

		if (mWindow != nullptr)
		{
			bs_delete(mWindow);
			mWindow = nullptr;
		}

		mProperties.mActive = false;
	}

	void D3D9RenderWindowCore::initialize()
	{
		RenderWindowCore::initialize();

		D3D9RenderWindowProperties& props = mProperties;

		mMultisampleType = D3DMULTISAMPLE_NONE;
		mMultisampleQuality = 0;
		mDisplayFrequency = Math::roundToInt(mDesc.videoMode.getRefreshRate());

		WINDOW_DESC windowDesc;
		windowDesc.border = mDesc.border;
		windowDesc.enableDoubleClick = mDesc.enableDoubleClick;
		windowDesc.fullscreen = mDesc.fullscreen;
		windowDesc.width = mDesc.videoMode.getWidth();
		windowDesc.height = mDesc.videoMode.getHeight();
		windowDesc.hidden = mDesc.hidden || mDesc.hideUntilSwap;
		windowDesc.left = mDesc.left;
		windowDesc.top = mDesc.top;
		windowDesc.outerDimensions = mDesc.outerDimensions;
		windowDesc.title = mDesc.title;
		windowDesc.toolWindow = mDesc.toolWindow;
		windowDesc.creationParams = this;
		windowDesc.module = mInstance;
		windowDesc.modal = mDesc.modal;
		windowDesc.wndProc = &Win32Platform::_win32WndProc;

		NameValuePairList::const_iterator opt;
		opt = mDesc.platformSpecific.find("parentWindowHandle");
		if (opt != mDesc.platformSpecific.end())
			windowDesc.parent = (HWND)parseUINT64(opt->second);

		opt = mDesc.platformSpecific.find("externalWindowHandle");
		if (opt != mDesc.platformSpecific.end())
			windowDesc.external = (HWND)parseUINT64(opt->second);

		mIsChild = windowDesc.parent != nullptr;
		props.mIsFullScreen = mDesc.fullscreen && !mIsChild;
		props.mColorDepth = 32;
		props.mActive = true;

		const D3D9VideoModeInfo& videoModeInfo = static_cast<const D3D9VideoModeInfo&>(RenderAPICore::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs > 0)
		{
			UINT32 actualMonitorIdx = std::min(mDesc.videoMode.getOutputIdx(), numOutputs - 1);
			const D3D9VideoOutputInfo& outputInfo = static_cast<const D3D9VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));
			windowDesc.monitor = outputInfo.getMonitorHandle();
		}

		if (!windowDesc.external)
		{
			mShowOnSwap = mDesc.hideUntilSwap;
			props.mHidden = mDesc.hideUntilSwap;
		}

		mWindow = bs_new<Win32Window>(windowDesc);

		props.mWidth = mWindow->getWidth();
		props.mHeight = mWindow->getHeight();
		props.mTop = mWindow->getTop();
		props.mLeft = mWindow->getLeft();

		mIsDepthBuffered = mDesc.depthBuffer;

		D3D9RenderAPI* rs = static_cast<D3D9RenderAPI*>(RenderAPICore::instancePtr());
		rs->registerWindow(*this);

		{
			ScopedSpinLock lock(mLock);
			mSyncedProperties = props;
		}

		RenderWindowManager::instance().notifySyncDataDirty(this);
	}

	void D3D9RenderWindowCore::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const D3D9VideoModeInfo& videoModeInfo = static_cast<const D3D9VideoModeInfo&>(RenderAPICore::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const D3D9VideoOutputInfo& outputInfo = static_cast<const D3D9VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		D3D9RenderWindowProperties& props = mProperties;

		props.mWidth = width;
		props.mHeight = height;
		mDisplayFrequency = Math::roundToInt(refreshRate);

		props.mIsFullScreen = true;

		HMONITOR hMonitor = outputInfo.getMonitorHandle();
		MONITORINFO monitorInfo;

		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		props.mTop = monitorInfo.rcMonitor.top;
		props.mLeft = monitorInfo.rcMonitor.left;

		// Invalidate device, which resets it
		mDevice->invalidate(this);
		mDevice->acquire();

		_windowMovedOrResized();
	}

	void D3D9RenderWindowCore::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		setFullscreen(mode.getWidth(), mode.getHeight(), mode.getRefreshRate(), mode.getOutputIdx());
	}

	void D3D9RenderWindowCore::setWindowed(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D9RenderWindowProperties& props = mProperties;

		if (!props.mIsFullScreen)
			return;

		props.mIsFullScreen = false;
		props.mWidth = width;
		props.mHeight = height;

		UINT32 winWidth = 0;
		UINT32 winHeight = 0;

		RECT rect;
		SetRect(&rect, 0, 0, winWidth, winHeight);

		AdjustWindowRect(&rect, mWindow->getStyle(), false);
		winWidth = rect.right - rect.left;
		winHeight = rect.bottom - rect.top;

		// Deal with centering when switching down to smaller resolution
		HMONITOR hMonitor = MonitorFromWindow(mWindow->getHWnd(), MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		memset(&monitorInfo, 0, sizeof(MONITORINFO));
		monitorInfo.cbSize = sizeof(MONITORINFO);
		GetMonitorInfo(hMonitor, &monitorInfo);

		LONG screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
		LONG screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

		int left = screenw > int(winWidth) ? ((screenw - int(winWidth)) / 2) : 0;
		int top = screenh > int(winHeight) ? ((screenh - int(winHeight)) / 2) : 0;

		SetWindowLong(mWindow->getHWnd(), GWL_STYLE, mWindow->getStyle());
		SetWindowLong(mWindow->getHWnd(), GWL_EXSTYLE, mWindow->getStyleEx());
		SetWindowPos(mWindow->getHWnd(), HWND_NOTOPMOST, left, top, winWidth, winHeight,
			SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);

		mDevice->invalidate(this);
		mDevice->acquire();

		_windowMovedOrResized();
	}

	void D3D9RenderWindowCore::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->setActive(state);

		RenderWindowCore::setActive(state);
	}

	void D3D9RenderWindowCore::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		mShowOnSwap = false;
		mWindow->setHidden(hidden);

		RenderWindowCore::setHidden(hidden);
	}

	void D3D9RenderWindowCore::minimize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->minimize();
	}

	void D3D9RenderWindowCore::maximize()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->maximize();
	}

	void D3D9RenderWindowCore::restore()
	{
		THROW_IF_NOT_CORE_THREAD;

		mWindow->restore();
	}

	void D3D9RenderWindowCore::move(INT32 left, INT32 top)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D9RenderWindowProperties& props = mProperties;

		if (!props.mIsFullScreen)
		{
			mWindow->move(left, top);

			props.mTop = mWindow->getTop();
			props.mLeft = mWindow->getLeft();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mLeft = props.mLeft;
				mSyncedProperties.mTop = props.mTop;
			}

			RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void D3D9RenderWindowCore::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D9RenderWindowProperties& props = mProperties;

		if (!props.mIsFullScreen)
		{
			mWindow->resize(width, height);

			props.mWidth = mWindow->getWidth();
			props.mHeight = mWindow->getHeight();

			{
				ScopedSpinLock lock(mLock);
				mSyncedProperties.mWidth = props.mWidth;
				mSyncedProperties.mHeight = props.mHeight;
			}

			RenderWindowManager::instance().notifySyncDataDirty(this);
		}
	}

	void D3D9RenderWindowCore::getCustomAttribute(const String& name, void* pData) const
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
			UINT64 *pHwnd = (UINT64*)pData;
			*pHwnd = (UINT64)_getWindowHandle();
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

	void D3D9RenderWindowCore::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mShowOnSwap)
			setHidden(false);

		if (mDeviceValid)
			mDevice->present(this);		
	}

	void D3D9RenderWindowCore::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		mDevice->copyContentsToMemory(this, dst, buffer);
	}

	void D3D9RenderWindowCore::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mWindow)
			return;

		mWindow->_windowMovedOrResized();

		D3D9RenderWindowProperties& props = mProperties;
		if (!props.isFullScreen()) // Fullscreen is handled directly by this object
		{
			props.mTop = mWindow->getTop();
			props.mLeft = mWindow->getLeft();
			props.mWidth = mWindow->getWidth();
			props.mHeight = mWindow->getHeight();
		}

		RenderWindowCore::_windowMovedOrResized();
	}

	/************************************************************************/
	/* 						D3D9 IMPLEMENTATION SPECIFIC                    */
	/************************************************************************/
	
	void D3D9RenderWindowCore::_buildPresentParameters(D3DPRESENT_PARAMETERS* presentParams) const
	{	
		const D3D9RenderWindowProperties& props = mProperties;
		IDirect3D9* pD3D = D3D9RenderAPI::getDirect3D9();
		D3DDEVTYPE devType = D3DDEVTYPE_HAL;

		if (mDevice != NULL)		
			devType = mDevice->getDeviceType();		
	
		ZeroMemory( presentParams, sizeof(D3DPRESENT_PARAMETERS) );
		presentParams->Windowed = !props.mIsFullScreen;
		presentParams->SwapEffect = D3DSWAPEFFECT_DISCARD;
		presentParams->BackBufferCount = 1;
		presentParams->EnableAutoDepthStencil = mIsDepthBuffered;
		presentParams->hDeviceWindow = mWindow->getHWnd();
		presentParams->BackBufferWidth = props.mWidth;
		presentParams->BackBufferHeight = props.mHeight;
		presentParams->FullScreen_RefreshRateInHz = props.mIsFullScreen ? mDisplayFrequency : 0;
		
		if (presentParams->BackBufferWidth == 0)		
			presentParams->BackBufferWidth = 1;					

		if (presentParams->BackBufferHeight == 0)	
			presentParams->BackBufferHeight = 1;					

		if (props.getVSync())
		{
			if (props.isFullScreen())
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

		D3D9RenderAPI* rs = static_cast<D3D9RenderAPI*>(BansheeEngine::RenderAPICore::instancePtr());

		D3DMULTISAMPLE_TYPE multisampleType;
		DWORD multisampleQuality;

		rs->determineMultisampleSettings(mDevice->getD3D9Device(), props.getMultisampleCount(),
			presentParams->BackBufferFormat, props.isFullScreen(), &multisampleType, &multisampleQuality);

		presentParams->MultiSampleType = multisampleType;
		presentParams->MultiSampleQuality = (multisampleQuality == 0) ? 0 : multisampleQuality;
	}

	HWND D3D9RenderWindowCore::_getWindowHandle() const
	{
		return mWindow->getHWnd();
	}

	IDirect3DDevice9* D3D9RenderWindowCore::_getD3D9Device() const
	{
		return mDevice->getD3D9Device();
	}

	IDirect3DSurface9* D3D9RenderWindowCore::_getRenderSurface() const
	{
		return mDevice->getBackBuffer(this);
	}

	D3D9Device* D3D9RenderWindowCore::_getDevice() const
	{
		return mDevice;
	}

	void D3D9RenderWindowCore::_setDevice(D3D9Device* device)
	{
		mDevice = device;
		mDeviceValid = false;
	}

	bool D3D9RenderWindowCore::_isDepthBuffered() const
	{
		return mIsDepthBuffered;
	}

	bool D3D9RenderWindowCore::_validateDevice()
	{
		mDeviceValid = mDevice->validate(this);
		return mDeviceValid;
	}

	void D3D9RenderWindowCore::syncProperties()
	{
		ScopedSpinLock lock(mLock);
		mProperties = mSyncedProperties;
	}

	D3D9RenderWindow::D3D9RenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, HINSTANCE instance)
		:RenderWindow(desc, windowId), mInstance(instance), mProperties(desc)
	{

	}

	void D3D9RenderWindow::getCustomAttribute(const String& name, void* pData) const
	{
		if (name == "WINDOW")
		{
			UINT64 *pHwnd = (UINT64*)pData;
			*pHwnd = (UINT64)getHWnd();
			return;
		}
	}

	Vector2I D3D9RenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I D3D9RenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(getHWnd(), &pos);
		return Vector2I(pos.x, pos.y);
	}

	HWND D3D9RenderWindow::getHWnd() const
	{
		// HACK: I'm accessing core method from sim thread, which means an invalid handle
		// could be returned here if requested too soon after initialization.
		return getCore()->_getWindowHandle();
	}

	SPtr<D3D9RenderWindowCore> D3D9RenderWindow::getCore() const
	{
		return std::static_pointer_cast<D3D9RenderWindowCore>(mCoreSpecific);
	}

	void D3D9RenderWindow::syncProperties()
	{
		ScopedSpinLock lock(getCore()->mLock);
		mProperties = getCore()->mSyncedProperties;
	}
}

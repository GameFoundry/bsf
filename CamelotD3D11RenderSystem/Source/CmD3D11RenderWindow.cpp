#include "CmD3D11RenderWindow.h"
#include "CmCoreThread.h"
#include "Win32/CmPlatformWndProc.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmD3D11RenderTexture.h"
#include "CmD3D11TextureView.h"
#include "CmTextureManager.h"
#include "CmD3D11DriverList.h"
#include "CmD3D11Driver.h"
#include "CmD3D11VideoModeInfo.h"
#include "CmInput.h"
#include "CmException.h"

namespace BansheeEngine
{
	D3D11RenderWindow::D3D11RenderWindow(const RENDER_WINDOW_DESC& desc,D3D11Device& device, IDXGIFactory* DXGIFactory)
		: RenderWindow(desc)
		, mDevice(device)
		, mDXGIFactory(DXGIFactory)
		, mIsExternal(false)
		, mSizing(false)
		, mClosed(false)
		, mRenderTargetView(nullptr)
		, mBackBuffer(nullptr)
		, mSwapChain(nullptr)
		, mHWnd(0)
		, mDepthStencilView(nullptr)
		, mIsChild(false)
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	}

	D3D11RenderWindow::~D3D11RenderWindow()
	{
	}

	void D3D11RenderWindow::initialize_internal()
	{
		mMultisampleType.Count = 1;
		mMultisampleType.Quality = 0;
		mMultisampleCount = 0;
		mMultisampleHint = "";
		mVSync = false;
		mVSyncInterval = 1;
		HWND parentHWnd = 0;
		HWND externalHandle = 0;
		HMONITOR hMonitor = NULL;

		// Get variable-length params
		NameValuePairList::const_iterator opt;

		// parentWindowHandle		-> parentHWnd
		opt = mDesc.platformSpecific.find("parentWindowHandle");
		if(opt != mDesc.platformSpecific.end())
			parentHWnd = (HWND)parseUnsignedInt(opt->second);
		// externalWindowHandle		-> externalHandle
		opt = mDesc.platformSpecific.find("externalWindowHandle");
		if(opt != mDesc.platformSpecific.end())
			externalHandle = (HWND)parseUnsignedInt(opt->second);
		// monitor handle
		opt = mDesc.platformSpecific.find("monitorHandle");
		if (opt != mDesc.platformSpecific.end())
			hMonitor = (HMONITOR)parseInt(opt->second);		

		mName = mDesc.title;
		mIsChild = parentHWnd != 0;
		mIsFullScreen = mDesc.fullscreen && !mIsChild;
		mColorDepth = mDesc.colorDepth;
		mWidth = mHeight = mLeft = mTop = 0;

		mActive = true;
		mClosed = false;

		if (!externalHandle)
		{
			DWORD dwStyle = (mHidden ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN;
			DWORD dwStyleEx = 0;
			RECT rc;
			MONITORINFO monitorInfo;

			// If we specified which adapter we want to use - find it's monitor.
			if (mDesc.monitorIndex != -1)
			{
				RenderSystem* rs = RenderSystem::instancePtr();
				D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(rs);

				D3D11DriverList* driverList = d3d11rs->getDriverList();

				UINT32 curOutput = 0;
				for(UINT32 i = 0; i < driverList->count(); i++)
				{
					D3D11Driver* driver = driverList->item(i);
					UINT32 numOutputs = driver->getNumAdapterOutputs();

					for(UINT32 j = 0; j < numOutputs; j++)
					{
						if(curOutput == mDesc.monitorIndex)
						{
							hMonitor = driver->getOutputDesc(j).Monitor;
							break;
						}

						curOutput++;
					}

					if(curOutput == mDesc.monitorIndex)
						break;
				}			
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
			winWidth = mDesc.width;
			winHeight = mDesc.height;

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
				else if (mDesc.monitorIndex != -1)
					left += monitorInfo.rcWork.left;

				if (top == -1)
					top = monitorInfo.rcWork.top + (screenh - outerh) / 2;
				else if (mDesc.monitorIndex != -1)
					top += monitorInfo.rcWork.top;
			}
			else if (mDesc.monitorIndex != -1)
			{
				left += monitorInfo.rcWork.left;
				top += monitorInfo.rcWork.top;
			}

			mWidth = mDesc.width;
			mHeight = mDesc.height;
			mTop = top;
			mLeft = left;

			if (!mDesc.fullscreen)
			{
				if (parentHWnd)
				{
					if(mDesc.toolWindow)
						dwStyleEx = WS_EX_TOOLWINDOW;
					else
						dwStyle |= WS_CHILD;
				}

				if (!parentHWnd || mDesc.toolWindow)
				{
					if (mDesc.border == WindowBorder::None)
						dwStyle |= WS_POPUP;
					else if (mDesc.border == WindowBorder::Fixed)
						dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
						WS_SYSMENU | WS_MINIMIZEBOX;
					else
						dwStyle |= WS_OVERLAPPEDWINDOW;
				}

				if (!mDesc.outerDimensions)
				{
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, mWidth, mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					mWidth = rc.right - rc.left;
					mHeight = rc.bottom - rc.top;

					// Clamp width and height to the desktop dimensions
					int screenw = GetSystemMetrics(SM_CXSCREEN);
					int screenh = GetSystemMetrics(SM_CYSCREEN);
					if ((int)mWidth > screenw)
						mWidth = screenw;
					if ((int)mHeight > screenh)
						mHeight = screenh;
					if (mLeft < 0)
						mLeft = (screenw - mWidth) / 2;
					if (mTop < 0)
						mTop = (screenh - mHeight) / 2;
				}
			}
			else
			{
				dwStyle |= WS_POPUP;
				mTop = mLeft = 0;
			}

			UINT classStyle = 0;
			if (mDesc.enableDoubleClick)
				classStyle |= CS_DBLCLKS;

			HINSTANCE hInst = NULL;

			// Register the window class
			// Allow 4 bytes of window data for D3D11RenderWindow pointer
			WNDCLASS wc = { classStyle, PlatformWndProc::_win32WndProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "D3D11Wnd" };	

			RegisterClass(&wc);

			// Create our main window
			// Pass pointer to self
			mIsExternal = false;
			mHWnd = CreateWindowEx(dwStyleEx, "D3D11Wnd", mDesc.title.c_str(), dwStyle,
				mLeft, mTop, mWidth, mHeight, parentHWnd, 0, hInst, this);
		}
		else
		{
			mHWnd = externalHandle;
			mIsExternal = true;
		}

		RECT rc;
		// top and left represent outer window coordinates
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;
		// width and height represent interior drawable area
		GetClientRect(mHWnd, &rc);
		mWidth = rc.right;
		mHeight = rc.bottom;

		createSwapChain();
		createSizeDependedD3DResources();
		mDXGIFactory->MakeWindowAssociation(mHWnd, NULL);
		setHidden(mHidden);

		RenderWindow::initialize_internal();
	}

	void D3D11RenderWindow::destroy_internal()
	{
		mActive = false;
		mClosed = true;

		SAFE_RELEASE(mSwapChain);

		if (mHWnd && !mIsExternal)
		{
			DestroyWindow(mHWnd);
		}

		if(mDepthStencilView != nullptr)
		{
			Texture::releaseView(mDepthStencilView);
			mDepthStencilView = nullptr;
		}

		mHWnd = nullptr;

		destroySizeDependedD3DResources();

		RenderWindow::destroy_internal();
	}

	void D3D11RenderWindow::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mDevice.getD3D11Device() != nullptr)
		{
			HRESULT hr = mSwapChain->Present(mVSync ? mVSyncInterval : 0, 0);

			if( FAILED(hr) )
				CM_EXCEPT(RenderingAPIException, "Error Presenting surfaces");
		}
	}

	void D3D11RenderWindow::move(INT32 top, INT32 left)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && !mIsFullScreen)
		{
			mTop = top;
			mLeft = left;

			SetWindowPos(mHWnd, 0, top, left, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void D3D11RenderWindow::resize(UINT32 width, UINT32 height)
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

	void D3D11RenderWindow::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mHWnd && mSwapChain)
		{
			if (state)
			{
				ShowWindow(mHWnd, SW_RESTORE);
				mSwapChain->SetFullscreenState(mIsFullScreen, nullptr);
			}
			else
			{
				ShowWindow(mHWnd, SW_SHOWMINIMIZED);
				mSwapChain->SetFullscreenState(FALSE, nullptr);
			}
		}

		RenderWindow::setActive(state);
	}

	void D3D11RenderWindow::setHidden(bool hidden)
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

	void D3D11RenderWindow::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderSystem::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		UINT32 actualMonitorIdx = std::min(monitorIdx, numOutputs - 1);
		const D3D11VideoOutputInfo& outputInfo = static_cast<const D3D11VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		DXGI_MODE_DESC modeDesc;
		ZeroMemory(&modeDesc, sizeof(modeDesc));

		modeDesc.Width = width;
		modeDesc.Height = height;
		modeDesc.RefreshRate.Numerator = Math::roundToInt(refreshRate);
		modeDesc.RefreshRate.Denominator = 1;
		modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		modeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_MODE_DESC nearestMode;
		ZeroMemory(&nearestMode, sizeof(nearestMode));

		outputInfo.getDXGIOutput()->FindClosestMatchingMode(&modeDesc, &nearestMode, nullptr);

		mIsFullScreen = true;
		mWidth = width;
		mHeight = height;

		mSwapChain->ResizeTarget(&nearestMode);
		mSwapChain->SetFullscreenState(true, outputInfo.getDXGIOutput()); 
	}

	void D3D11RenderWindow::setFullscreen(const VideoMode& mode)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (mIsChild)
			return;

		if (mode.isCustom())
		{
			setFullscreen(mode.getWidth(), mode.getHeight(), mode.getRefreshRate(), mode.getOutputIdx());
			return;
		}

		const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderSystem::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs == 0)
			return;

		UINT32 actualMonitorIdx = std::min(mode.getOutputIdx(), numOutputs - 1);
		const D3D11VideoOutputInfo& outputInfo = static_cast<const D3D11VideoOutputInfo&>(videoModeInfo.getOutputInfo(actualMonitorIdx));

		const D3D11VideoMode& videoMode = static_cast<const D3D11VideoMode&>(mode);

		mIsFullScreen = true;
		mWidth = mode.getWidth();
		mHeight = mode.getHeight();

		mSwapChain->ResizeTarget(&videoMode.getDXGIModeDesc());
		mSwapChain->SetFullscreenState(true, outputInfo.getDXGIOutput());
	}

	void D3D11RenderWindow::setWindowed()
	{
		THROW_IF_NOT_CORE_THREAD;

		mIsFullScreen = false;
		mSwapChainDesc.Windowed = false;
		mSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		mSwapChainDesc.BufferDesc.Width = mWidth;
		mSwapChainDesc.BufferDesc.Height = mHeight;

		DXGI_MODE_DESC modeDesc;
		ZeroMemory(&modeDesc, sizeof(modeDesc));

		modeDesc.Width = mWidth;
		modeDesc.Height = mHeight;
		modeDesc.RefreshRate.Numerator = 0;
		modeDesc.RefreshRate.Denominator = 0;
		modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		mSwapChain->ResizeTarget(&modeDesc);
		mSwapChain->SetFullscreenState(false, nullptr);
	}

	void D3D11RenderWindow::getCustomAttribute( const String& name, void* pData ) const
	{
		if(name == "WINDOW")
		{
			HWND *pWnd = (HWND*)pData;
			*pWnd = mHWnd;
			return;
		}

		if(name == "RTV")
		{
			*static_cast<ID3D11RenderTargetView**>(pData) = mRenderTargetView;
			return;
		}
		else if(name == "DSV")
		{
			D3D11TextureView* d3d11TextureView = static_cast<D3D11TextureView*>(mDepthStencilView.get());
			*static_cast<ID3D11DepthStencilView**>(pData) = d3d11TextureView->getDSV();
			return;
		}

		RenderWindow::getCustomAttribute(name, pData);
	}

	void D3D11RenderWindow::copyToMemory(const PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mBackBuffer == nullptr)
			return;

		// get the backbuffer desc
		D3D11_TEXTURE2D_DESC BBDesc;
		mBackBuffer->GetDesc(&BBDesc);

		ID3D11Texture2D *backbuffer = nullptr;

		if(BBDesc.SampleDesc.Quality > 0)
		{
			D3D11_TEXTURE2D_DESC desc = BBDesc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.BindFlags = 0;
			desc.SampleDesc.Quality = 0;
			desc.SampleDesc.Count = 1;

			HRESULT hr = mDevice.getD3D11Device()->CreateTexture2D(
				&desc,
				NULL,
				&backbuffer);

			if (FAILED(hr) || mDevice.hasError())
			{
				String errorDescription = mDevice.getErrorDescription();
				CM_EXCEPT(RenderingAPIException,
					"Error creating texture\nError Description:" + errorDescription);
			}

			mDevice.getImmediateContext()->ResolveSubresource(backbuffer, D3D11CalcSubresource(0, 0, 1), mBackBuffer, D3D11CalcSubresource(0, 0, 1), desc.Format);
		}


		// change the parameters of the texture so we can read it
		BBDesc.Usage = D3D11_USAGE_STAGING;
		BBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		BBDesc.BindFlags = 0;
		BBDesc.SampleDesc.Quality = 0;
		BBDesc.SampleDesc.Count = 1;

		// create a temp buffer to copy to
		ID3D11Texture2D * pTempTexture2D;
		HRESULT hr = mDevice.getD3D11Device()->CreateTexture2D(
			&BBDesc,
			NULL,
			&pTempTexture2D);

		if (FAILED(hr) || mDevice.hasError())
		{
			String errorDescription = mDevice.getErrorDescription();
			CM_EXCEPT(RenderingAPIException,
				"Error creating texture\nError Description:" + errorDescription);
		}
		// copy the back buffer
		mDevice.getImmediateContext()->CopyResource(pTempTexture2D, backbuffer != NULL ? backbuffer : mBackBuffer);

		// map the copied texture
		D3D11_MAPPED_SUBRESOURCE mappedTex2D;
		mDevice.getImmediateContext()->Map(pTempTexture2D, 0,D3D11_MAP_READ, 0, &mappedTex2D);

		// copy the the texture to the dest
		PixelData src(mWidth, mHeight, 1, PF_A8B8G8R8);
		src.setExternalBuffer((UINT8*)mappedTex2D.pData);
		PixelUtil::bulkPixelConversion(src, dst);

		// unmap the temp buffer
		mDevice.getImmediateContext()->Unmap(pTempTexture2D, 0);

		// Release the temp buffer
		SAFE_RELEASE(pTempTexture2D);
		SAFE_RELEASE(backbuffer);
	}

	Vector2I D3D11RenderWindow::screenToWindowPos(const Vector2I& screenPos) const
	{
		POINT pos;
		pos.x = screenPos.x;
		pos.y = screenPos.y;

		ScreenToClient(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	Vector2I D3D11RenderWindow::windowToScreenPos(const Vector2I& windowPos) const
	{
		POINT pos;
		pos.x = windowPos.x;
		pos.y = windowPos.y;

		ClientToScreen(mHWnd, &pos);
		return Vector2I(pos.x, pos.y);
	}

	void D3D11RenderWindow::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mHWnd || IsIconic(mHWnd))
			return;

		RECT rc;
		// top and left represent outer window position
		GetWindowRect(mHWnd, &rc);
		mTop = rc.top;
		mLeft = rc.left;
		// width and height represent drawable area only
		GetClientRect(mHWnd, &rc);
		unsigned int width = rc.right - rc.left;
		unsigned int height = rc.bottom - rc.top;

		if (width == 0) 
			width = 1;
		if (height == 0)
			height = 1;

		resizeSwapChainBuffers(width, height);

		RenderWindow::_windowMovedOrResized();
	}

	void D3D11RenderWindow::createSwapChain()
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		// get the dxgi device
		IDXGIDevice* pDXGIDevice = queryDxgiDevice();

		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mSwapChainDesc.OutputWindow			= mHWnd;
		mSwapChainDesc.BufferDesc.Width		= mWidth;
		mSwapChainDesc.BufferDesc.Height	= mHeight;
		mSwapChainDesc.BufferDesc.Format	= format;
		mSwapChainDesc.BufferDesc.RefreshRate.Numerator=0;
		mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;

		mSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		mSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		mSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;

		mSwapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		mSwapChainDesc.BufferCount			= 1;
		mSwapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD ;

		mSwapChainDesc.OutputWindow 		= mHWnd;
		mSwapChainDesc.Windowed				= !mIsFullScreen;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		rs->determineMultisampleSettings(mMultisampleCount, mMultisampleHint, format, &mMultisampleType);
		mSwapChainDesc.SampleDesc.Count = mMultisampleType.Count;
		mSwapChainDesc.SampleDesc.Quality = mMultisampleType.Quality;

		HRESULT hr;

		// Create swap chain			
		hr = mDXGIFactory->CreateSwapChain(pDXGIDevice, &mSwapChainDesc, &mSwapChain);

		if (FAILED(hr))
		{
			// Try a second time, may fail the first time due to back buffer count,
			// which will be corrected by the runtime
			hr = mDXGIFactory->CreateSwapChain(pDXGIDevice, &mSwapChainDesc, &mSwapChain);
		}

		SAFE_RELEASE(pDXGIDevice);

		if (FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Unable to create swap chain");
	}

	void D3D11RenderWindow::createSizeDependedD3DResources()
	{
		// obtain back buffer
		SAFE_RELEASE(mBackBuffer);

		HRESULT hr = mSwapChain->GetBuffer(0,  __uuidof(ID3D11Texture2D), (LPVOID*)&mBackBuffer);
		if( FAILED(hr) )
			CM_EXCEPT(RenderingAPIException, "Unable to Get Back Buffer for swap chain");

		// create all other size depended resources
		assert(mBackBuffer && !mRenderTargetView);

		// get the backbuffer desc
		D3D11_TEXTURE2D_DESC BBDesc;
		mBackBuffer->GetDesc(&BBDesc);

		// create the render target view
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory( &RTVDesc, sizeof(RTVDesc) );

		RTVDesc.Format = BBDesc.Format;
		RTVDesc.ViewDimension = mMultisampleCount ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		hr = mDevice.getD3D11Device()->CreateRenderTargetView(mBackBuffer, &RTVDesc, &mRenderTargetView);

		if( FAILED(hr) )
		{
			String errorDescription = mDevice.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Unable to create rendertagert view\nError Description:" + errorDescription);
		}

		mDepthStencilBuffer = TextureManager::instance().createTexture(TEX_TYPE_2D, 
			BBDesc.Width, BBDesc.Height, 0, PF_D24S8, TU_DEPTHSTENCIL, false, mMultisampleCount, mMultisampleHint);

		if(mDepthStencilView != nullptr)
		{
			Texture::releaseView(mDepthStencilView);
			mDepthStencilView = nullptr;
		}

		mDepthStencilView = Texture::requestView(mDepthStencilBuffer, 0, 1, 0, 1, GVU_DEPTHSTENCIL);
	}

	void D3D11RenderWindow::destroySizeDependedD3DResources()
	{
		SAFE_RELEASE(mBackBuffer);
		SAFE_RELEASE(mRenderTargetView);

		mDepthStencilBuffer = nullptr;
	}

	void D3D11RenderWindow::resizeSwapChainBuffers(UINT32 width, UINT32 height)
	{
		destroySizeDependedD3DResources();

		// width and height can be zero to autodetect size, therefore do not rely on them
		UINT Flags = mIsFullScreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
		HRESULT hr = mSwapChain->ResizeBuffers(mSwapChainDesc.BufferCount, width, height, mSwapChainDesc.BufferDesc.Format, Flags);

		if(hr != S_OK)
			CM_EXCEPT(InternalErrorException, "Call to ResizeBuffers failed.");

		mSwapChain->GetDesc(&mSwapChainDesc);
		mWidth = mSwapChainDesc.BufferDesc.Width;
		mHeight = mSwapChainDesc.BufferDesc.Height;
		mIsFullScreen = (0 == mSwapChainDesc.Windowed); // Alt-Enter together with SetWindowAssociation() can change this state

		createSizeDependedD3DResources();

		mDevice.getImmediateContext()->OMSetRenderTargets(0, 0, 0);
	}

	IDXGIDevice* D3D11RenderWindow::queryDxgiDevice()
	{
		if (mDevice.getD3D11Device() == nullptr)
		{
			CM_EXCEPT(RenderingAPIException, "D3D11Device is NULL!");
		}

		IDXGIDevice* pDXGIDevice = nullptr;
		HRESULT hr = mDevice.getD3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
		if(FAILED(hr))
			CM_EXCEPT(RenderingAPIException, "Unable to query a DXGIDevice");

		return pDXGIDevice;
	}

	bool D3D11RenderWindow::checkMultiSampleQuality(UINT32 SampleCount, UINT32 *outQuality, DXGI_FORMAT format)
	{
		if (SUCCEEDED(mDevice.getD3D11Device()->CheckMultisampleQualityLevels(format, SampleCount, outQuality)))
			return true;
		else
			return false;
	}
}
#include "BsD3D11RenderWindow.h"
#include "BsCoreThread.h"
#include "Win32/BsPlatformWndProc.h"
#include "BsD3D11RenderSystem.h"
#include "BsD3D11Device.h"
#include "BsD3D11RenderTexture.h"
#include "BsD3D11TextureView.h"
#include "BsTextureManager.h"
#include "BsD3D11DriverList.h"
#include "BsD3D11Driver.h"
#include "BsD3D11VideoModeInfo.h"
#include "BsRenderStats.h"
#include "BsInput.h"
#include "BsException.h"

namespace BansheeEngine
{
	D3D11RenderWindowCore::D3D11RenderWindowCore(D3D11RenderWindow* parent, RenderWindowProperties* properties, const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory* DXGIFactory)
		: RenderWindowCore(parent, properties), mDevice(device), mDXGIFactory(DXGIFactory), mIsExternal(false), mSizing(false),
		 mRenderTargetView(nullptr), mBackBuffer(nullptr), mSwapChain(nullptr), mHWnd(0), 
		mDepthStencilView(nullptr), mIsChild(false), mRefreshRateNumerator(0), mRefreshRateDenominator(0)
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		D3D11RenderWindowProperties* props = static_cast<D3D11RenderWindowProperties*>(mProperties);

		mMultisampleType.Count = 1;
		mMultisampleType.Quality = 0;
		HWND parentHWnd = 0;
		HWND externalHandle = 0;


		NameValuePairList::const_iterator opt;
		opt = desc.platformSpecific.find("parentWindowHandle");
		if (opt != desc.platformSpecific.end())
			parentHWnd = (HWND)parseUnsignedInt(opt->second);

		opt = desc.platformSpecific.find("externalWindowHandle");
		if (opt != desc.platformSpecific.end())
			externalHandle = (HWND)parseUnsignedInt(opt->second);

		props->mName = desc.title;
		mIsChild = parentHWnd != 0;
		props->mIsFullScreen = desc.fullscreen && !mIsChild;
		props->mColorDepth = 32;

		props->mActive = true;

		if (desc.videoMode.isCustom())
		{
			mRefreshRateNumerator = Math::roundToInt(desc.videoMode.getRefreshRate());
			mRefreshRateDenominator = 1;
		}
		else
		{
			const D3D11VideoMode& d3d11videoMode = static_cast<const D3D11VideoMode&>(desc.videoMode);
			mRefreshRateNumerator = d3d11videoMode.getRefreshRateNumerator();
			mRefreshRateDenominator = d3d11videoMode.getRefreshRateDenominator();
		}

		HMONITOR hMonitor = NULL;
		const D3D11VideoOutputInfo* outputInfo = nullptr;

		const D3D11VideoModeInfo& videoModeInfo = static_cast<const D3D11VideoModeInfo&>(RenderSystem::instance().getVideoModeInfo());
		UINT32 numOutputs = videoModeInfo.getNumOutputs();
		if (numOutputs > 0)
		{
			UINT32 actualMonitorIdx = std::min(desc.videoMode.getOutputIdx(), numOutputs - 1);
			outputInfo = static_cast<const D3D11VideoOutputInfo*>(&videoModeInfo.getOutputInfo(actualMonitorIdx));

			DXGI_OUTPUT_DESC desc;
			outputInfo->getDXGIOutput()->GetDesc(&desc);

			hMonitor = desc.Monitor;
		}

		if (!externalHandle)
		{
			DWORD dwStyle = (getProperties().isHidden() ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN;
			DWORD dwStyleEx = 0;
			RECT rc;
			MONITORINFO monitorInfo;

			// If we didn't specified the adapter index, or if it didn't find it
			if (hMonitor == NULL)
			{
				POINT windowAnchorPoint;

				// Fill in anchor point.
				windowAnchorPoint.x = desc.left;
				windowAnchorPoint.y = desc.top;

				// Get the nearest monitor to this window.
				hMonitor = MonitorFromPoint(windowAnchorPoint, MONITOR_DEFAULTTOPRIMARY);
			}

			// Get the target monitor info		
			memset(&monitorInfo, 0, sizeof(MONITORINFO));
			monitorInfo.cbSize = sizeof(MONITORINFO);
			GetMonitorInfo(hMonitor, &monitorInfo);

			unsigned int winWidth, winHeight;
			winWidth = desc.videoMode.getWidth();
			winHeight = desc.videoMode.getHeight();

			UINT32 left = desc.left;
			UINT32 top = desc.top;

			// No specified top left -> Center the window in the middle of the monitor
			if (left == -1 || top == -1)
			{
				int screenw = monitorInfo.rcWork.right - monitorInfo.rcWork.left;
				int screenh = monitorInfo.rcWork.bottom - monitorInfo.rcWork.top;

				// clamp window dimensions to screen size
				int outerw = (int(winWidth) < screenw) ? int(winWidth) : screenw;
				int outerh = (int(winHeight) < screenh) ? int(winHeight) : screenh;

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

			props->mWidth = desc.videoMode.getWidth();
			props->mHeight = desc.videoMode.getHeight();
			props->mTop = top;
			props->mLeft = left;

			if (!desc.fullscreen)
			{
				if (parentHWnd)
				{
					if (desc.toolWindow)
						dwStyleEx = WS_EX_TOOLWINDOW;
					else
						dwStyle |= WS_CHILD;
				}

				if (!parentHWnd || desc.toolWindow)
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
					// Calculate window dimensions required
					// to get the requested client area
					SetRect(&rc, 0, 0, props->mWidth, props->mHeight);
					AdjustWindowRect(&rc, dwStyle, false);
					props->mWidth = rc.right - rc.left;
					props->mHeight = rc.bottom - rc.top;

					// Clamp width and height to the desktop dimensions
					int screenw = GetSystemMetrics(SM_CXSCREEN);
					int screenh = GetSystemMetrics(SM_CYSCREEN);
					if ((int)props->mWidth > screenw)
						props->mWidth = screenw;
					if ((int)props->mHeight > screenh)
						props->mHeight = screenh;
					if (props->mLeft < 0)
						props->mLeft = (screenw - props->mWidth) / 2;
					if (props->mTop < 0)
						props->mTop = (screenh - props->mHeight) / 2;
				}
			}
			else
			{
				dwStyle |= WS_POPUP;
				props->mTop = 0;
				props->mLeft = 0;
			}

			UINT classStyle = 0;
			if (desc.enableDoubleClick)
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
			mHWnd = CreateWindowEx(dwStyleEx, "D3D11Wnd", desc.title.c_str(), dwStyle,
				props->mLeft, props->mTop, props->mWidth, props->mHeight, parentHWnd, 0, hInst, this);
		}
		else
		{
			mHWnd = externalHandle;
			mIsExternal = true;
		}

		RECT rc;
		GetWindowRect(mHWnd, &rc);
		props->mTop = rc.top;
		props->mLeft = rc.left;

		GetClientRect(mHWnd, &rc);
		props->mWidth = rc.right;
		props->mHeight = rc.bottom;

		createSwapChain();

		if (getProperties().isFullScreen())
		{
			if (outputInfo != nullptr)
				mSwapChain->SetFullscreenState(true, outputInfo->getDXGIOutput());
			else
				mSwapChain->SetFullscreenState(true, nullptr);
		}

		createSizeDependedD3DResources();
		mDXGIFactory->MakeWindowAssociation(mHWnd, NULL);
		setHidden(getProperties().isHidden());
	}

	D3D11RenderWindowCore::~D3D11RenderWindowCore()
	{
		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);

		properties->mActive = false;
		markCoreDirty();

		SAFE_RELEASE(mSwapChain);
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_SwapChain);

		if (mHWnd && !mIsExternal)
		{
			DestroyWindow(mHWnd);
		}

		if (mDepthStencilView != nullptr)
		{
			Texture::releaseView(mDepthStencilView);
			mDepthStencilView = nullptr;
		}

		mHWnd = nullptr;

		destroySizeDependedD3DResources();
	}

	void D3D11RenderWindowCore::swapBuffers()
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mDevice.getD3D11Device() != nullptr)
		{
			HRESULT hr = mSwapChain->Present(getProperties().getVSync() ? getProperties().getVSyncInterval() : 0, 0);

			if( FAILED(hr) )
				BS_EXCEPT(RenderingAPIException, "Error Presenting surfaces");
		}
	}

	void D3D11RenderWindowCore::move(INT32 top, INT32 left)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);

		if (mHWnd && !properties->mIsFullScreen)
		{
			properties->mTop = top;
			properties->mLeft = left;

			SetWindowPos(mHWnd, 0, top, left, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			markCoreDirty();			
		}
	}

	void D3D11RenderWindowCore::resize(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);

		if (mHWnd && !properties->mIsFullScreen)
		{
			properties->mWidth = width;
			properties->mHeight = height;

			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;

			SetWindowPos(mHWnd, 0, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			markCoreDirty();
		}
	}

	void D3D11RenderWindowCore::setActive(bool state)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);

		if (mHWnd && mSwapChain)
		{
			if (state)
			{
				ShowWindow(mHWnd, SW_RESTORE);
				mSwapChain->SetFullscreenState(properties->mIsFullScreen, nullptr);
			}
			else
			{
				ShowWindow(mHWnd, SW_SHOWMINIMIZED);
				mSwapChain->SetFullscreenState(FALSE, nullptr);
			}
		}

		RenderWindowCore::setActive(state);
	}

	void D3D11RenderWindowCore::setHidden(bool hidden)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);

		properties->mHidden = hidden;
		if (!mIsExternal)
		{
			if (hidden)
				ShowWindow(mHWnd, SW_HIDE);
			else
				ShowWindow(mHWnd, SW_SHOWNORMAL);
		}

		markCoreDirty();
	}

	void D3D11RenderWindowCore::setFullscreen(UINT32 width, UINT32 height, float refreshRate, UINT32 monitorIdx)
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

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);
		properties->mIsFullScreen = true;
		properties->mWidth = width;
		properties->mHeight = height;

		mSwapChain->ResizeTarget(&nearestMode);
		mSwapChain->SetFullscreenState(true, outputInfo.getDXGIOutput()); 

		markCoreDirty();
	}

	void D3D11RenderWindowCore::setFullscreen(const VideoMode& mode)
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

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);
		properties->mIsFullScreen = true;
		properties->mWidth = mode.getWidth();
		properties->mHeight = mode.getHeight();

		mSwapChain->ResizeTarget(&videoMode.getDXGIModeDesc());
		mSwapChain->SetFullscreenState(true, outputInfo.getDXGIOutput());

		markCoreDirty();
	}

	void D3D11RenderWindowCore::setWindowed(UINT32 width, UINT32 height)
	{
		THROW_IF_NOT_CORE_THREAD;

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);
		properties->mWidth = width;
		properties->mHeight = height;
		properties->mIsFullScreen = false;

		mSwapChainDesc.Windowed = true;
		mSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		mSwapChainDesc.BufferDesc.Width = width;
		mSwapChainDesc.BufferDesc.Height = height;

		DXGI_MODE_DESC modeDesc;
		ZeroMemory(&modeDesc, sizeof(modeDesc));

		modeDesc.Width = width;
		modeDesc.Height = height;
		modeDesc.RefreshRate.Numerator = 0;
		modeDesc.RefreshRate.Denominator = 0;
		modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		mSwapChain->SetFullscreenState(false, nullptr);
		mSwapChain->ResizeTarget(&modeDesc);

		markCoreDirty();
	}

	void D3D11RenderWindowCore::getCustomAttribute(const String& name, void* pData) const
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

		RenderWindowCore::getCustomAttribute(name, pData);
	}

	void D3D11RenderWindowCore::copyToMemory(PixelData &dst, FrameBuffer buffer)
	{
		THROW_IF_NOT_CORE_THREAD;

		if(mBackBuffer == nullptr)
			return;

		// Get the backbuffer desc
		D3D11_TEXTURE2D_DESC BBDesc;
		mBackBuffer->GetDesc(&BBDesc);

		ID3D11Texture2D* backbuffer = nullptr;

		if(BBDesc.SampleDesc.Quality > 0)
		{
			D3D11_TEXTURE2D_DESC desc = BBDesc;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.CPUAccessFlags = 0;
			desc.BindFlags = 0;
			desc.SampleDesc.Quality = 0;
			desc.SampleDesc.Count = 1;

			HRESULT hr = mDevice.getD3D11Device()->CreateTexture2D(&desc, nullptr, &backbuffer);

			if (FAILED(hr) || mDevice.hasError())
			{
				String errorDescription = mDevice.getErrorDescription();
				BS_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
			}

			mDevice.getImmediateContext()->ResolveSubresource(backbuffer, D3D11CalcSubresource(0, 0, 1), mBackBuffer, D3D11CalcSubresource(0, 0, 1), desc.Format);
		}

		// Change the parameters of the texture so we can read it
		BBDesc.Usage = D3D11_USAGE_STAGING;
		BBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		BBDesc.BindFlags = 0;
		BBDesc.SampleDesc.Quality = 0;
		BBDesc.SampleDesc.Count = 1;

		// Create a temp buffer to copy to
		ID3D11Texture2D* tempTexture;
		HRESULT hr = mDevice.getD3D11Device()->CreateTexture2D(&BBDesc, nullptr, &tempTexture);

		if (FAILED(hr) || mDevice.hasError())
		{
			String errorDescription = mDevice.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Error creating texture\nError Description:" + errorDescription);
		}

		// Copy the back buffer
		mDevice.getImmediateContext()->CopyResource(tempTexture, backbuffer != NULL ? backbuffer : mBackBuffer);

		// Map the copied texture
		D3D11_MAPPED_SUBRESOURCE mappedTex2D;
		mDevice.getImmediateContext()->Map(tempTexture, 0,D3D11_MAP_READ, 0, &mappedTex2D);

		// Copy the the texture to the dest
		PixelData src(getProperties().getWidth(), getProperties().getHeight(), 1, PF_A8B8G8R8);
		src.setExternalBuffer((UINT8*)mappedTex2D.pData);
		PixelUtil::bulkPixelConversion(src, dst);

		// Unmap the temp buffer
		mDevice.getImmediateContext()->Unmap(tempTexture, 0);

		// Release the temp buffer
		SAFE_RELEASE(tempTexture);
		SAFE_RELEASE(backbuffer);
	}

	void D3D11RenderWindowCore::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mHWnd || IsIconic(mHWnd))
			return;

		RECT rc;
		GetWindowRect(mHWnd, &rc);

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);
		properties->mTop = rc.top;
		properties->mLeft = rc.left;
		markCoreDirty();

		GetClientRect(mHWnd, &rc);
		unsigned int width = rc.right - rc.left;
		unsigned int height = rc.bottom - rc.top;

		if (width == 0)
			width = 1;

		if (height == 0)
			height = 1;

		resizeSwapChainBuffers(width, height);

		RenderWindowCore::_windowMovedOrResized();
	}

	void D3D11RenderWindowCore::createSwapChain()
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		IDXGIDevice* pDXGIDevice = queryDxgiDevice();

		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
		mSwapChainDesc.OutputWindow	= mHWnd;
		mSwapChainDesc.BufferDesc.Width = getProperties().getWidth();
		mSwapChainDesc.BufferDesc.Height = getProperties().getHeight();
		mSwapChainDesc.BufferDesc.Format = format;

		if (getProperties().isFullScreen())
		{
			mSwapChainDesc.BufferDesc.RefreshRate.Numerator = mRefreshRateNumerator;
			mSwapChainDesc.BufferDesc.RefreshRate.Denominator = mRefreshRateDenominator;
		}
		else
		{
			mSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			mSwapChainDesc.BufferDesc.RefreshRate.Denominator = 0;
		}

		mSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		mSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		mSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH ;

		mSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		mSwapChainDesc.BufferCount = 1;
		mSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD ;

		mSwapChainDesc.Windowed	= true;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		rs->determineMultisampleSettings(getProperties().getMultisampleCount(), getProperties().getMultisampleHint(), format, &mMultisampleType);
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
			BS_EXCEPT(RenderingAPIException, "Unable to create swap chain");

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_SwapChain);
	}

	void D3D11RenderWindowCore::createSizeDependedD3DResources()
	{
		SAFE_RELEASE(mBackBuffer);

		HRESULT hr = mSwapChain->GetBuffer(0,  __uuidof(ID3D11Texture2D), (LPVOID*)&mBackBuffer);
		if(FAILED(hr))
			BS_EXCEPT(RenderingAPIException, "Unable to Get Back Buffer for swap chain");

		assert(mBackBuffer && !mRenderTargetView);

		D3D11_TEXTURE2D_DESC BBDesc;
		mBackBuffer->GetDesc(&BBDesc);

		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory( &RTVDesc, sizeof(RTVDesc) );

		RTVDesc.Format = BBDesc.Format;
		RTVDesc.ViewDimension = getProperties().getMultisampleCount() ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		hr = mDevice.getD3D11Device()->CreateRenderTargetView(mBackBuffer, &RTVDesc, &mRenderTargetView);

		if(FAILED(hr))
		{
			String errorDescription = mDevice.getErrorDescription();
			BS_EXCEPT(RenderingAPIException, "Unable to create rendertagert view\nError Description:" + errorDescription);
		}

		mDepthStencilBuffer = TextureManager::instance().createTexture(TEX_TYPE_2D, 
			BBDesc.Width, BBDesc.Height, 0, PF_D24S8, TU_DEPTHSTENCIL, false, 
			getProperties().getMultisampleCount(), getProperties().getMultisampleHint());

		if(mDepthStencilView != nullptr)
		{
			Texture::releaseView(mDepthStencilView);
			mDepthStencilView = nullptr;
		}

		mDepthStencilView = Texture::requestView(mDepthStencilBuffer, 0, 1, 0, 1, GVU_DEPTHSTENCIL);
	}

	void D3D11RenderWindowCore::destroySizeDependedD3DResources()
	{
		SAFE_RELEASE(mBackBuffer);
		SAFE_RELEASE(mRenderTargetView);

		mDepthStencilBuffer = nullptr;
	}

	void D3D11RenderWindowCore::resizeSwapChainBuffers(UINT32 width, UINT32 height)
	{
		destroySizeDependedD3DResources();

		UINT Flags = getProperties().isFullScreen() ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
		HRESULT hr = mSwapChain->ResizeBuffers(mSwapChainDesc.BufferCount, width, height, mSwapChainDesc.BufferDesc.Format, Flags);

		if(hr != S_OK)
			BS_EXCEPT(InternalErrorException, "Call to ResizeBuffers failed.");

		D3D11RenderWindowProperties* properties = static_cast<D3D11RenderWindowProperties*>(mProperties);
		mSwapChain->GetDesc(&mSwapChainDesc);
		properties->mWidth = mSwapChainDesc.BufferDesc.Width;
		properties->mHeight = mSwapChainDesc.BufferDesc.Height;
		properties->mIsFullScreen = (0 == mSwapChainDesc.Windowed); // Alt-Enter together with SetWindowAssociation() can change this state
		markCoreDirty();

		createSizeDependedD3DResources();

		mDevice.getImmediateContext()->OMSetRenderTargets(0, 0, 0);
	}

	IDXGIDevice* D3D11RenderWindowCore::queryDxgiDevice()
	{
		if (mDevice.getD3D11Device() == nullptr)
		{
			BS_EXCEPT(RenderingAPIException, "D3D11Device is null.");
		}

		IDXGIDevice* pDXGIDevice = nullptr;
		HRESULT hr = mDevice.getD3D11Device()->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

		if(FAILED(hr))
			BS_EXCEPT(RenderingAPIException, "Unable to query a DXGIDevice.");

		return pDXGIDevice;
	}

	D3D11RenderWindow::D3D11RenderWindow(D3D11Device& device, IDXGIFactory* DXGIFactory)
		:mDevice(device), mDXGIFactory(DXGIFactory), mHWnd(0)
	{

	}

	void D3D11RenderWindow::getCustomAttribute(const String& name, void* pData) const
	{
		THROW_IF_CORE_THREAD;

		if (name == "WINDOW")
		{
			HWND *pWnd = (HWND*)pData;
			*pWnd = mHWnd;
			return;
		}

		RenderWindow::getCustomAttribute(name, pData);
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

	void D3D11RenderWindow::initialize_internal()
	{
		RenderWindow::initialize_internal();

		mCore->getCustomAttribute("WINDOW", (void*)&mHWnd);
	}

	RenderTargetProperties* D3D11RenderWindow::createProperties() const
	{
		return bs_new<RenderWindowProperties>();
	}

	RenderWindowCore* D3D11RenderWindow::createCore(RenderWindowProperties* properties, const RENDER_WINDOW_DESC& desc)
	{
		return bs_new<D3D11RenderWindowCore>(this, properties, desc, mDevice, mDXGIFactory);
	}
}
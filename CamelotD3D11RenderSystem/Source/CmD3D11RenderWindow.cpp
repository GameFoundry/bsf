#include "CmD3D11RenderWindow.h"
#include "CmWindowEventUtilities.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11Device.h"
#include "CmDepthStencilBuffer.h"
#include "CmTextureManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11RenderWindow::D3D11RenderWindow(D3D11Device& device, IDXGIFactory* DXGIFactory)
		: RenderWindow()
		, mDevice(device)
		, mDXGIFactory(DXGIFactory)
		, mIsExternal(false)
		, mSizing(false)
		, mClosed(false)
		, mHidden(false)
		, mSwitchingFullscreen(false)
		, mDisplayFrequency(0)
		, mRenderTargetView(nullptr)
		, mBackBuffer(nullptr)
		, mSwapChain(nullptr)
		, mHWnd(0)
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	}

	D3D11RenderWindow::~D3D11RenderWindow()
	{
		destroy();
	}

	void D3D11RenderWindow::initialize(const String& name, unsigned int width, unsigned int height,
		bool fullScreen, const NameValuePairList *miscParams)
	{
		mFSAAType.Count = 1;
		mFSAAType.Quality = 0;
		mFSAA = 0;
		mFSAAHint = "";
		mVSync = false;
		mVSyncInterval = 1;
		HWND parentHWnd = 0;
		HWND externalHandle = 0;
		String title = name;
		int left = -1; // Defaults to screen center
		int top = -1; // Defaults to screen center
		String border = "";
		bool outerSize = false;
		bool enableDoubleClick = false;

		unsigned int colourDepth = 32;
		bool depthBuffer = true;

		if(miscParams)
		{
			// Get variable-length params
			NameValuePairList::const_iterator opt;
			// vsync	[parseBool]
			opt = miscParams->find("vsync");
			if(opt != miscParams->end())
				mVSync = parseBool(opt->second);
			// vsyncInterval	[parseUnsignedInt]
			opt = miscParams->find("vsyncInterval");
			if(opt != miscParams->end())
				mVSyncInterval = parseUnsignedInt(opt->second);
			// hidden	[parseBool]
			opt = miscParams->find("hidden");
			if(opt != miscParams->end())
				mHidden = parseBool(opt->second);
			// displayFrequency
			opt = miscParams->find("displayFrequency");
			if(opt != miscParams->end())
				mDisplayFrequency = parseUnsignedInt(opt->second);
			// colourDepth
			opt = miscParams->find("colourDepth");
			if(opt != miscParams->end())
				colourDepth = parseUnsignedInt(opt->second);
			// depthBuffer [parseBool]
			opt = miscParams->find("depthBuffer");
			if(opt != miscParams->end())
				depthBuffer = parseBool(opt->second);
			// FSAA type
			opt = miscParams->find("FSAA");
			if(opt != miscParams->end())
				mFSAA = parseUnsignedInt(opt->second);
			// FSAA quality
			opt = miscParams->find("FSAAHint");
			if(opt != miscParams->end())
				mFSAAHint = opt->second;
			// sRGB?
			opt = miscParams->find("gamma");
			if(opt != miscParams->end())
				mHwGamma = parseBool(opt->second);
			// left (x)
			opt = miscParams->find("left");
			if(opt != miscParams->end())
				left = parseInt(opt->second);
			// top (y)
			opt = miscParams->find("top");
			if(opt != miscParams->end())
				top = parseInt(opt->second);
			// Window title
			opt = miscParams->find("title");
			if(opt != miscParams->end())
				title = opt->second;
			// parentWindowHandle		-> parentHWnd
			opt = miscParams->find("parentWindowHandle");
			if(opt != miscParams->end())
				parentHWnd = (HWND)parseUnsignedInt(opt->second);
			// externalWindowHandle		-> externalHandle
			opt = miscParams->find("externalWindowHandle");
			if(opt != miscParams->end())
				externalHandle = (HWND)parseUnsignedInt(opt->second);
			// window border style
			opt = miscParams->find("border");
			if(opt != miscParams->end())
				border = opt->second;
			// set outer dimensions?
			opt = miscParams->find("outerDimensions");
			if(opt != miscParams->end())
				outerSize = parseBool(opt->second);
			// enable double click messages
			opt = miscParams->find("enableDoubleClick");
			if(opt != miscParams->end())
				enableDoubleClick = parseBool(opt->second);
		}

		mName = name;
		mIsFullScreen = fullScreen;
		mColorDepth = colourDepth;
		mWidth = mHeight = mLeft = mTop = 0;

		mActive = true;
		mClosed = false;

		// Destroy current window if any
		if(mHWnd)
			destroy();

		if (!externalHandle)
		{
			DWORD dwStyle = (mHidden ? 0 : WS_VISIBLE) | WS_CLIPCHILDREN;
			RECT rc;

			mWidth = width;
			mHeight = height;
			mTop = top;
			mLeft = left;

			if (!fullScreen)
			{
				if (parentHWnd)
				{
					dwStyle |= WS_CHILD;
				}
				else
				{
					if (border == "none")
						dwStyle |= WS_POPUP;
					else if (border == "fixed")
						dwStyle |= WS_OVERLAPPED | WS_BORDER | WS_CAPTION |
						WS_SYSMENU | WS_MINIMIZEBOX;
					else
						dwStyle |= WS_OVERLAPPEDWINDOW;
				}

				if (!outerSize)
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
			if (enableDoubleClick)
				classStyle |= CS_DBLCLKS;

			HINSTANCE hInst = NULL;

			// Register the window class
			// Allow 4 bytes of window data for D3D11RenderWindow pointer
			WNDCLASS wc = { classStyle, WindowEventUtilities::_WndProc, 0, 0, hInst,
				LoadIcon(0, IDI_APPLICATION), LoadCursor(NULL, IDC_ARROW),
				(HBRUSH)GetStockObject(BLACK_BRUSH), 0, "D3D11Wnd" };	


			RegisterClass(&wc);

			// Create our main window
			// Pass pointer to self
			mIsExternal = false;
			mHWnd = CreateWindow("D3D11Wnd", title.c_str(), dwStyle,
				mLeft, mTop, mWidth, mHeight, parentHWnd, 0, hInst, this);

			WindowEventUtilities::_addRenderWindow(this);
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

		_createSwapChain();
		_createSizeDependedD3DResources();
		mDXGIFactory->MakeWindowAssociation(mHWnd, NULL);
		setHidden(mHidden);
	}

	void D3D11RenderWindow::destroy()
	{
		_destroySizeDependedD3DResources();

		mActive = false;
		mClosed = true;

		SAFE_RELEASE(mSwapChain);

		if (mHWnd && !mIsExternal)
		{
			WindowEventUtilities::_removeRenderWindow(this);
			DestroyWindow(mHWnd);
		}

		mHWnd = nullptr;
	}

	void D3D11RenderWindow::swapBuffers(bool waitForVSync)
	{
		if(mDevice.getD3D11Device() != nullptr)
		{
			HRESULT hr = mSwapChain->Present(waitForVSync ? mVSyncInterval : 0, 0);

			if( FAILED(hr) )
				CM_EXCEPT(RenderingAPIException, "Error Presenting surfaces");
		}
	}

	void D3D11RenderWindow::reposition(int top, int left)
	{
		if (mHWnd && !mIsFullScreen)
		{
			SetWindowPos(mHWnd, 0, top, left, 0, 0,
				SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void D3D11RenderWindow::resize(unsigned int width, unsigned int height)
	{
		if (mHWnd && !mIsFullScreen)
		{
			RECT rc = { 0, 0, width, height };
			AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
			width = rc.right - rc.left;
			height = rc.bottom - rc.top;
			SetWindowPos(mHWnd, 0, 0, 0, width, height,
				SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
	}

	void D3D11RenderWindow::windowMovedOrResized()
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
		unsigned int width = rc.right - rc.left;
		unsigned int height = rc.bottom - rc.top;

		if (width == 0) 
			width = 1;
		if (height == 0)
			height = 1;

		if (mWidth == width && mHeight == height)
			return;

		_resizeSwapChainBuffers(width, height);
	}

	void D3D11RenderWindow::setActive(bool state)
	{
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
		mHidden = hidden;
		if (!mIsExternal)
		{
			if (hidden)
				ShowWindow(mHWnd, SW_HIDE);
			else
				ShowWindow(mHWnd, SW_SHOWNORMAL);
		}
	}

	void D3D11RenderWindow::setFullscreen(bool fullScreen, unsigned int width, unsigned int height)
	{
		if (fullScreen != mIsFullScreen || width != mWidth || height != mHeight)
		{
			if (fullScreen != mIsFullScreen)
				mSwitchingFullscreen = true;

			DWORD dwStyle = WS_VISIBLE | WS_CLIPCHILDREN;

			bool oldFullscreen = mIsFullScreen;
			mIsFullScreen = fullScreen;

			if (fullScreen)
			{
				dwStyle |= WS_POPUP;
				mTop = mLeft = 0;
				mWidth = width;
				mHeight = height;
				// need different ordering here

				if (oldFullscreen)
				{
					// was previously fullscreen, just changing the resolution
					SetWindowPos(mHWnd, HWND_TOPMOST, 0, 0, width, height, SWP_NOACTIVATE);
				}
				else
				{
					SetWindowPos(mHWnd, HWND_TOPMOST, 0, 0, width, height, SWP_NOACTIVATE);
					//MoveWindow(mHWnd, mLeft, mTop, mWidth, mHeight, FALSE);
					SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
					SetWindowPos(mHWnd, 0, 0,0, 0,0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
				}
			}
			else
			{
				dwStyle |= WS_OVERLAPPEDWINDOW;
				// Calculate window dimensions required
				// to get the requested client area
				RECT rc;
				SetRect(&rc, 0, 0, width, height);
				AdjustWindowRect(&rc, dwStyle, false);
				unsigned int winWidth = rc.right - rc.left;
				unsigned int winHeight = rc.bottom - rc.top;

				SetWindowLong(mHWnd, GWL_STYLE, dwStyle);
				SetWindowPos(mHWnd, HWND_NOTOPMOST, 0, 0, winWidth, winHeight,
					SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOACTIVATE);
				// Note that we also set the position in the restoreLostDevice method
				// via _finishSwitchingFullScreen
			}

			mSwapChainDesc.Windowed = !fullScreen;
			mSwapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			mSwapChainDesc.BufferDesc.RefreshRate.Denominator=0;
			mSwapChainDesc.BufferDesc.Height = height;
			mSwapChainDesc.BufferDesc.Width = width;
		}
	} 

	void D3D11RenderWindow::getCustomAttribute( const String& name, void* pData )
	{
		// Valid attributes and their equvalent native functions:
		// D3DDEVICE			: getD3DDevice
		// WINDOW				: getWindowHandle

		if(name == "WINDOW")
		{
			HWND *pWnd = (HWND*)pData;
			*pWnd = mHWnd;
			return;
		}

		if(name == "D3DDEVICE")
		{
			ID3D11Device **device = (ID3D11Device **)pData;
			*device = mDevice.getD3D11Device();
			return;
		}
		else if(name == "isTexture")
		{
			bool *b = reinterpret_cast< bool * >( pData );
			*b = false;
			return;
		}
		else if(name == "ID3D11RenderTargetView")
		{
			*static_cast<ID3D11RenderTargetView**>(pData) = mRenderTargetView;
			return;
		}
		else if(name == "ID3D11Texture2D")
		{
			ID3D11Texture2D **pBackBuffer = (ID3D11Texture2D**)pData;
			*pBackBuffer = mBackBuffer;
			return;
		}
		else if(name == "numberOfViews")
		{
			unsigned int* n = reinterpret_cast<unsigned int*>(pData);
			*n = 1;
			return;
		}
		else if(name == "DDBACKBUFFER")
		{
			ID3D11Texture2D **ppBackBuffer = (ID3D11Texture2D**) pData;
			ppBackBuffer[0] = NULL;
			return;
		}

		RenderWindow::getCustomAttribute_internal(name, pData);
	}

	void D3D11RenderWindow::copyContentsToMemory(const PixelData &dst, FrameBuffer buffer)
	{
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
		PixelUtil::bulkPixelConversion(PixelData(mWidth, mHeight, 1, PF_A8B8G8R8, mappedTex2D.pData), dst);

		// unmap the temp buffer
		mDevice.getImmediateContext()->Unmap(pTempTexture2D, 0);

		// Release the temp buffer
		SAFE_RELEASE(pTempTexture2D);
		SAFE_RELEASE(backbuffer);
	}

	void D3D11RenderWindow::_createSwapChain()
	{
		ZeroMemory(&mSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		// get the dxgi device
		IDXGIDevice* pDXGIDevice = _queryDxgiDevice();

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

		// triple buffer if VSync is on
		mSwapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
		mSwapChainDesc.BufferCount			= mVSync ? 2 : 1;
		mSwapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_DISCARD ;

		mSwapChainDesc.OutputWindow 		= mHWnd;
		mSwapChainDesc.Windowed				= !mIsFullScreen;

		D3D11RenderSystem* rs = static_cast<D3D11RenderSystem*>(RenderSystem::instancePtr());
		rs->determineFSAASettings(mFSAA, mFSAAHint, format, &mFSAAType);
		mSwapChainDesc.SampleDesc.Count = mFSAAType.Count;
		mSwapChainDesc.SampleDesc.Quality = mFSAAType.Quality;

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

	void D3D11RenderWindow::_createSizeDependedD3DResources()
	{
		// obtain back buffer
		SAFE_RELEASE(mBackBuffer);

		HRESULT hr = mSwapChain->GetBuffer(0,  __uuidof( ID3D11Texture2D ), (LPVOID*)&mBackBuffer);
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
		RTVDesc.ViewDimension = mFSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		hr = mDevice.getD3D11Device()->CreateRenderTargetView(mBackBuffer, &RTVDesc, &mRenderTargetView);

		if( FAILED(hr) )
		{
			String errorDescription = mDevice.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, "Unable to create rendertagert view\nError Description:" + errorDescription);
		}

		mDepthStencilBuffer = TextureManager::instance().createDepthStencilBuffer(DFMT_D24S8, BBDesc.Width, BBDesc.Height, mFSAA, mFSAAHint);
	}

	void D3D11RenderWindow::_destroySizeDependedD3DResources()
	{
		SAFE_RELEASE(mBackBuffer);
		SAFE_RELEASE(mRenderTargetView);

		mDepthStencilBuffer = nullptr;
	}

	void D3D11RenderWindow::_resizeSwapChainBuffers(unsigned width, unsigned height)
	{
		_destroySizeDependedD3DResources();

		// width and height can be zero to autodetect size, therefore do not rely on them
		UINT Flags = mIsFullScreen ? DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 0;
		mSwapChain->ResizeBuffers(mSwapChainDesc.BufferCount, width, height, mSwapChainDesc.BufferDesc.Format, Flags);
		mSwapChain->GetDesc(&mSwapChainDesc);
		mWidth = mSwapChainDesc.BufferDesc.Width;
		mHeight = mSwapChainDesc.BufferDesc.Height;
		mIsFullScreen = (0 == mSwapChainDesc.Windowed); // Alt-Enter together with SetWindowAssociation() can change this state

		_createSizeDependedD3DResources();

		mDevice.getImmediateContext()->OMSetRenderTargets(0, 0, 0);
		// Additional swap chains need their own depth buffer
		// to support resizing them

		HRESULT hr = mSwapChain->GetBuffer( 0,  __uuidof( ID3D11Texture2D ), (LPVOID*)&mBackBuffer  );
		if( FAILED(hr) )
		{
			CM_EXCEPT(RenderingAPIException, 
				"Unable to Get Back Buffer for swap chain");
		}

		// get the backbuffer desc
		D3D11_TEXTURE2D_DESC BBDesc;
		mBackBuffer->GetDesc(&BBDesc);

		// create the render target view
		D3D11_RENDER_TARGET_VIEW_DESC RTVDesc;
		ZeroMemory( &RTVDesc, sizeof(RTVDesc) );

		RTVDesc.Format = BBDesc.Format;
		RTVDesc.ViewDimension = mFSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
		RTVDesc.Texture2D.MipSlice = 0;
		hr = mDevice.getD3D11Device()->CreateRenderTargetView(mBackBuffer, &RTVDesc, &mRenderTargetView);

		if(FAILED(hr))
		{
			String errorDescription = mDevice.getErrorDescription();
			CM_EXCEPT(RenderingAPIException, 
				"Unable to create rendertagert view\nError Description:" + errorDescription);
		}
	}

	IDXGIDevice* D3D11RenderWindow::_queryDxgiDevice()
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

	void D3D11RenderWindow::_finishSwitchingFullscreen()
	{
		if(mIsFullScreen)
		{
			// Need to reset the region on the window sometimes, when the 
			// windowed mode was constrained by desktop 
			HRGN hRgn = CreateRectRgn(0, 0, mSwapChainDesc.BufferDesc.Width, mSwapChainDesc.BufferDesc.Height);
			SetWindowRgn(mHWnd, hRgn, FALSE);
		}
		else
		{
			// When switching back to windowed mode, need to reset window size 
			// after device has been restored
			RECT rc;
			SetRect(&rc, 0, 0, mSwapChainDesc.BufferDesc.Width, mSwapChainDesc.BufferDesc.Height);
			AdjustWindowRect(&rc, GetWindowLong(mHWnd, GWL_STYLE), false);
			unsigned int winWidth = rc.right - rc.left;
			unsigned int winHeight = rc.bottom - rc.top;
			int screenw = GetSystemMetrics(SM_CXSCREEN);
			int screenh = GetSystemMetrics(SM_CYSCREEN);
			int left = (screenw - winWidth) / 2;
			int top = (screenh - winHeight) / 2;
			SetWindowPos(mHWnd, HWND_NOTOPMOST, left, top, winWidth, winHeight,
				SWP_DRAWFRAME | SWP_FRAMECHANGED | SWP_NOACTIVATE);
		}

		mSwapChain->SetFullscreenState(mIsFullScreen, NULL);
		mSwitchingFullscreen = false;
	}

	bool D3D11RenderWindow::_checkMultiSampleQuality(UINT SampleCount, UINT *outQuality, DXGI_FORMAT format)
	{
		if (SUCCEEDED(mDevice.getD3D11Device()->CheckMultisampleQualityLevels(format, SampleCount, outQuality)))
			return true;
		else
			return false;
	}
}
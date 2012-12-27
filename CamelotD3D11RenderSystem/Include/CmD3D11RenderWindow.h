#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderWindow.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderWindow : public RenderWindow
	{
	public:
		D3D11RenderWindow(D3D11Device& device, IDXGIFactory* DXGIFactory);
		~D3D11RenderWindow();

		void initialize(const String& name, unsigned width, unsigned height, bool fullScreen, 
			const NameValuePairList* miscParams);
		void destroy(void);

		void reposition(int left, int top);
		void resize(unsigned int width, unsigned int height);

		void setHidden(bool hidden);
		void setActive(bool state);
		void setFullscreen(bool fullScreen, unsigned int width, unsigned int height);

		void swapBuffers(bool waitForVSync = true);
		void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer);

		void windowMovedOrResized();

		bool isClosed() const									{ return mClosed; }
		bool isHidden() const									{ return mHidden; }

		void getCustomAttribute( const String& name, void* pData );
		DXGI_SWAP_CHAIN_DESC* getPresentationParameters(void)	{ return &mSwapChainDesc; }
		HWND getWindowHandle() const							{ return mHWnd; }

		bool requiresTextureFlipping() const					{ return false; }

	protected:
		void _createSizeDependedD3DResources();
		void _destroySizeDependedD3DResources();

		IDXGIDevice* _queryDxgiDevice(); 
	
		bool _checkMultiSampleQuality(UINT SampleCount, UINT *outQuality, DXGI_FORMAT format);

		void _createSwapChain();
		void _resizeSwapChainBuffers(unsigned width, unsigned height);

		bool _getSwitchingFullscreen() const					{ return mSwitchingFullscreen; }
		void _finishSwitchingFullscreen();
		
	protected:
		D3D11Device&	mDevice;			// D3D11 driver
		IDXGIFactory*	mDXGIFactory;
		bool	mIsExternal;
		bool	mSizing;
		bool	mClosed;
		bool	mHidden;

		// -------------------------------------------------------
		// DirectX-specific
		// -------------------------------------------------------
		DXGI_SAMPLE_DESC mFSAAType;
		UINT mDisplayFrequency;
		bool mVSync;
		unsigned int mVSyncInterval;

		// Window size depended resources - must be released before swapchain resize and recreated later
		ID3D11Texture2D*			mBackBuffer;
		ID3D11RenderTargetView*		mRenderTargetView;
		ID3D11DepthStencilView*		mDepthStencilView;

		IDXGISwapChain*				mSwapChain;
		DXGI_SWAP_CHAIN_DESC		mSwapChainDesc;

		HWND	mHWnd;					// Win32 window handle
		bool	mSwitchingFullscreen;	// Are we switching from fullscreen to windowed or vice versa
	};
}
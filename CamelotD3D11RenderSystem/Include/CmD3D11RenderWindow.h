#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderWindow.h"

namespace CamelotFramework
{
	class CM_D3D11_EXPORT D3D11RenderWindow : public RenderWindow
	{
	public:
		~D3D11RenderWindow();

		void reposition(int left, int top);
		void resize(unsigned int width, unsigned int height);

		void setHidden(bool hidden);
		void setActive(bool state);
		void setFullscreen(bool fullScreen, unsigned int width, unsigned int height);

		void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer);
		void swapBuffers();

		void windowMovedOrResized();

		bool isClosed() const									{ return mClosed; }
		bool isHidden() const									{ return mHidden; }

		void getCustomAttribute( const String& name, void* pData ) const;
		DXGI_SWAP_CHAIN_DESC* getPresentationParameters(void)	{ return &mSwapChainDesc; }
		HWND getWindowHandle() const							{ return mHWnd; }

		bool requiresTextureFlipping() const					{ return false; }

	protected:
		friend class D3D11RenderWindowManager;
		D3D11RenderWindow(const RENDER_WINDOW_DESC& desc, D3D11Device& device, IDXGIFactory* DXGIFactory);

		void _createSizeDependedD3DResources();
		void _destroySizeDependedD3DResources();

		IDXGIDevice* _queryDxgiDevice(); 

		bool _checkMultiSampleQuality(UINT SampleCount, UINT *outQuality, DXGI_FORMAT format);

		void _createSwapChain();
		void _resizeSwapChainBuffers(unsigned width, unsigned height);

		bool _getSwitchingFullscreen() const					{ return mSwitchingFullscreen; }
		void _finishSwitchingFullscreen();
		
		/**
		 * @copydoc RenderWindow::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc RenderWindow::destroy_internal().
		 */
		void destroy_internal();
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
		TextureViewPtr				mDepthStencilView;
		TexturePtr					mDepthStencilBuffer;

		IDXGISwapChain*				mSwapChain;
		DXGI_SWAP_CHAIN_DESC		mSwapChainDesc;

		HWND	mHWnd;					// Win32 window handle
		bool	mSwitchingFullscreen;	// Are we switching from fullscreen to windowed or vice versa
	};
}
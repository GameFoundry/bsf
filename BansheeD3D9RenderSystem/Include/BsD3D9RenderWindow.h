#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsRenderWindow.h"
#include "BsD3D9Device.h"

namespace BansheeEngine 
{
	class D3D9RenderWindow;

	/**
	 * @brief	Contains various properties that describe a render window.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindowProperties : public RenderWindowProperties
	{
	public:
		D3D9RenderWindowProperties(const RENDER_WINDOW_DESC& desc);
		virtual ~D3D9RenderWindowProperties() { }

	private:
		friend class D3D9RenderWindowCore;
		friend class D3D9RenderWindow;
	};

	/**
	 * @brief	Render window implementation for Windows.
	 *
	 * @note	Core thread only.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindowCore : public RenderWindowCore
	{
	public:
		D3D9RenderWindowCore(const RENDER_WINDOW_DESC& desc, UINT32 windowId, HINSTANCE instance);
		~D3D9RenderWindowCore();
		
		/**
		 * @copydoc RenderWindowCore::setFullscreen(UINT32, UINT32, float, UINT32)
		 */
		void setFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0);

		/**
		 * @copydoc RenderWindowCore::setFullscreen(const VideoMode&)
		 */
		void setFullscreen(const VideoMode& mode);

		/**
		* @copydoc RenderWindowCore::setWindowed
		*/
		void setWindowed(UINT32 width, UINT32 height);

		/**
		 * @copydoc RenderWindowCore::setHidden
		 */
		void setHidden(bool hidden);

		/**
		 * @copydoc	RenderWindowCore::minimize
		 */
		void minimize();

		/**
		 * @copydoc	RenderWindowCore::maximize
		 */
		void maximize();

		/**
		 * @copydoc	RenderWindowCore::restore
		 */
		void restore();

		/**
		 * @copydoc RenderWindowCore::move
		 */
		void move(INT32 left, INT32 top);

		/**
		 * @copydoc RenderWindowCore::resize
		 */
		void resize(UINT32 width, UINT32 height);

		/**
		 * @copydoc RenderWindowCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc RenderWindowCore::copyContentsToMemory
		 */
		void copyToMemory(PixelData &dst, FrameBuffer buffer);

		/**
		 * @copydoc RenderWindowCore::swapBuffers
		 */
		void swapBuffers();

		/**
		 * @copydoc RenderWindowCore::_windowMovedOrResized
		 */
		void _windowMovedOrResized();

		/**
		 * @brief	Gets internal Win32 window handle.
		 */
		HWND _getWindowHandle() const;

		/**
		 * @brief	Gets the DirectX 9 device object that manages this window.
		 */
		IDirect3DDevice9* _getD3D9Device() const;

		/**
		 * @brief	Gets the device that manages this window.
		 */
		D3D9Device* _getDevice() const;

		/**
		 * @brief	Sets the device that manages this window.
		 */
		void _setDevice(D3D9Device* device);

		/**
		 * @brief	Build the presentation parameters used with this window.
		 */
		void _buildPresentParameters(D3DPRESENT_PARAMETERS* presentParams) const;

		/**
		 * @brief	Accessor for render surface.
		 */
		IDirect3DSurface9* _getRenderSurface() const;
	
		/**
		 * @brief	Returns true if this window use depth buffer.
		 */
		bool _isDepthBuffered() const;

		/**
		 * @brief	Validate the device for this window.
		 */
		bool _validateDevice();
		
	protected:
		friend class D3D9RenderWindow;

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @brief	Updates window coordinates and size from actual values provided by Windows.
		 */
		void updateWindowRect();

		/**
		 * @brief	Calculates window size based on provided client area size and currently set window style. 
		 */	
		void getAdjustedWindowSize(UINT32 clientWidth, UINT32 clientHeight,
			DWORD style, UINT32* winWidth, UINT32* winHeight);

		/**
		 * @copydoc	RenderWindowCore::getProperties
		 */
		const RenderTargetProperties& getPropertiesInternal() const { return mProperties; }

		/**
		 * @copydoc	RenderWindowCore::getSyncedProperties
		 */
		RenderWindowProperties& getSyncedProperties() override { return mSyncedProperties; }

		/**
		 * @copydoc	RenderWindowCore::syncProperties
		 */
		void syncProperties() override;

	protected:
		HINSTANCE mInstance;
		D3D9Device* mDevice;
		bool mDeviceValid;
		bool mIsExternal;		
		D3DMULTISAMPLE_TYPE	mMultisampleType;
		DWORD mMultisampleQuality;
		UINT mDisplayFrequency;
		unsigned int mVSyncInterval;		
		DWORD mStyle;	
		DWORD mWindowedStyle;
		DWORD mWindowedStyleEx;
		bool mIsDepthBuffered;
		bool mIsChild;
		HWND mHWnd;
		bool mShowOnSwap;

		D3D9RenderWindowProperties mProperties;
		D3D9RenderWindowProperties mSyncedProperties;
	};

	/**
	 * @brief	Render window implementation for Windows.
	 *
	 * @note	Sim thread only.
	 */
	class BS_D3D9_EXPORT D3D9RenderWindow : public RenderWindow
	{
	public:
		~D3D9RenderWindow() { }

		/**
		 * @copydoc RenderWindow::screenToWindowPos
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc RenderWindow::screenToWindowPos
		 */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @copydoc	RenderWindow::getCore
		 */
		SPtr<D3D9RenderWindowCore> getCore() const;

	protected:
		friend class D3D9RenderWindowManager;
		friend class D3D9RenderWindowCore;

		D3D9RenderWindow(const RENDER_WINDOW_DESC& desc, UINT32 windowId, HINSTANCE instance);

		/**
		 * @copydoc	RenderWindowCore::getProperties
		 */
		const RenderTargetProperties& getPropertiesInternal() const { return mProperties; }

		/**
		 * @copydoc	RenderWindow::syncProperties
		 */
		void syncProperties() override;

		/**
		 * @brief	Retrieves internal window handle.
		 */
		HWND getHWnd() const;

	private:
		HINSTANCE mInstance;
		D3D9RenderWindowProperties mProperties;
	};
}
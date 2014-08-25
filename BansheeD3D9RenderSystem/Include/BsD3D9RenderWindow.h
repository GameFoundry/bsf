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
		D3D9RenderWindowCore(D3D9RenderWindow* parent, RenderWindowProperties* properties, const RENDER_WINDOW_DESC& desc, HINSTANCE instance);
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
		HWND _getWindowHandle() const { return mHWnd; }

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
		 * @brief	Updates window coordinates and size from actual values provided by Windows.
		 */
		void updateWindowRect();

		/**
		 * @brief	Calculates window size based on provided client area size and currently set window style. 
		 */	
		void getAdjustedWindowSize(UINT32 clientWidth, UINT32 clientHeight,
			DWORD style, UINT32* winWidth, UINT32* winHeight);

	protected:
		HINSTANCE mInstance;
		D3D9Device* mDevice;
		bool mDeviceValid;
		HWND mHWnd;
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
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @copydoc RenderWindow::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc RenderWindow::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc	RenderWindow::getCore
		 */
		D3D9RenderWindowCore* getCore() const;

	protected:
		friend class D3D9RenderWindowManager;

		D3D9RenderWindow(HINSTANCE instance);

		/**
		 * @copydoc	RenderWindow::initialize_internal
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc	RenderWindow::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderWindow::createCore
		 */
		virtual RenderWindowCore* createCore(RenderWindowProperties* properties, const RENDER_WINDOW_DESC& desc);

	private:
		HINSTANCE mInstance;
		HWND mHWnd;
	};
}
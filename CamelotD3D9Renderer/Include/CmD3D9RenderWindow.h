#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmRenderWindow.h"
#include "CmD3D9Device.h"

namespace BansheeEngine 
{
	class CM_D3D9_EXPORT D3D9RenderWindow : public RenderWindow
	{
	public:
		~D3D9RenderWindow();
		
		/**
		 * @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32)
		 */
		void setFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0);

		/**
		 * @copydoc RenderWindow::setFullscreen(const VideoMode&, UINT32)
		 */
		void setFullscreen(const VideoMode& mode, UINT32 refreshRateIdx = 0);

		/**
		* @copydoc RenderWindow::setWindowed
		*/
		void setWindowed();

		/**
		 * @copydoc RenderWindow::setHidden
		 */
		void setHidden(bool hidden);

		/**
		 * @copydoc RenderWindow::isActive
		 */
		bool isActive() const;

		/**
		 * @copydoc RenderWindow::isVisible
		 */
		bool isVisible() const;

		/**
		 * @copydoc RenderWindow::isClosed
		 */
		bool isClosed() const { return mClosed; }

		/**
		 * @copydoc RenderWindow::isVSync
		 */
		bool isVSync() const { return mVSync; }

		/**
		 * @copydoc RenderWindow::reposition
		 */
		void move(INT32 left, INT32 top);

		/**
		 * @copydoc RenderWindow::resize
		 */
		void resize(UINT32 width, UINT32 height);

		/**
		 * @copydoc RenderWindow::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc RenderWindow::copyContentsToMemory
		 */
		void copyToMemory(const PixelData &dst, FrameBuffer buffer);

		/**
		 * @copydoc RenderWindow::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc RenderWindow::swapBuffers
		 */
		void swapBuffers();

		/**
		 * @copydoc RenderWindow::screenToWindowPos
		 */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @copydoc RenderWindow::_windowMovedOrResized
		 */
		void _windowMovedOrResized();

		HWND _getWindowHandle() const { return mHWnd; }				
		IDirect3DDevice9* _getD3D9Device() const;
		D3D9Device* _getDevice() const;
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

		void _adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
			DWORD style, unsigned int* winWidth, unsigned int* winHeight);

	protected:
		friend class D3D9RenderWindowManager;

		D3D9RenderWindow (const RENDER_WINDOW_DESC& desc, HINSTANCE instance);

		void updateWindowRect();

		/**
		 * @copydoc RenderWindow::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc RenderWindow::destroy_internal().
		 */
		void destroy_internal();

	protected:
		HINSTANCE					mInstance;				// Process instance
		D3D9Device* 				mDevice;				// D3D9 device wrapper class.
		bool						mDeviceValid;			// Device was validation succeeded.
		HWND						mHWnd;					// Win32 Window handle		
		bool						mIsExternal;			// window not created by Ogre
		bool						mClosed;				// Is this window destroyed.			
		D3DMULTISAMPLE_TYPE			mFSAAType;				// AA type.
		DWORD						mFSAAQuality;			// AA quality.
		UINT						mDisplayFrequency;		// Display frequency.
		bool						mVSync;					// Use vertical sync or not.
		unsigned int				mVSyncInterval;		
		DWORD						mStyle;					// Window style currently used for this window.
		bool						mIsDepthBuffered;
		bool						mIsChild;
		// Desired width / height after resizing
		unsigned int mDesiredWidth;
		unsigned int mDesiredHeight;
	};
}
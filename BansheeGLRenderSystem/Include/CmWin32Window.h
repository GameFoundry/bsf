#pragma once

#include "CmWin32Prerequisites.h"
#include "CmRenderWindow.h"

namespace BansheeEngine 
{
    class BS_RSGL_EXPORT Win32Window : public RenderWindow
    {
    public:
        ~Win32Window();

		/**
		 * @copydoc RenderWindow::setFullscreen(UINT32, UINT32, float, UINT32)
		 */
		void setFullscreen(UINT32 width, UINT32 height, float refreshRate = 60.0f, UINT32 monitorIdx = 0);

		/**
		 * @copydoc RenderWindow::setFullscreen(const VideoMode&)
		 */
		void setFullscreen(const VideoMode& mode);

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
		bool isClosed() const;

		/**
		 * @copydoc RenderWindow::reposition
		 */
		void move(INT32 left, INT32 top);

		/**
		 * @copydoc RenderWindow::resize
		 */
		void resize(UINT32 width, UINT32 height);

		/**
		 * @copydoc RenderWindow::copyContentsToMemory
		 */
		void copyToMemory(const PixelData &dst, FrameBuffer buffer);

		/**
		 * @copydoc RenderWindow::swapBuffers
		 */
		void swapBuffers();

		/**
		 * @copydoc RenderWindow::requiresTextureFlipping
		 */
		bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc RenderWindow::screenToWindowPos
		 */
		Vector2I screenToWindowPos(const Vector2I& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Vector2I windowToScreenPos(const Vector2I& windowPos) const;

		/**
		 * @copydoc RenderWindow::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc RenderWindow::setActive
		 */
		virtual void setActive( bool state );

		/**
		 * @copydoc RenderWindow::_windowMovedOrResized
		 */
		void _windowMovedOrResized(void);

		HWND _getWindowHandle() const { return mHWnd; }
		HDC _getHDC() const { return mHDC; }
		
		void _adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
			unsigned int* winWidth, unsigned int* winHeight);

	protected:
		friend class GLRenderWindowManager;
		friend class Win32GLSupport;

		Win32Window(const RENDER_WINDOW_DESC& desc, Win32GLSupport &glsupport);

		/**
		 * @copydoc RenderWindow::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc RenderWindow::destroy_internal().
		 */
		void destroy_internal();

		protected:
			Win32GLSupport &mGLSupport;
			HWND mHWnd;					// Win32 Window handle
			HDC	mHDC;
			DWORD mWindowedStyle;
			DWORD mWindowedStyleEx;
			bool mIsExternal;
			bool mIsChild;
			char* mDeviceName;
			bool mIsExternalGLControl;
			bool mSizing;
			bool mClosed;
			int mDisplayFrequency;      // fullscreen only, to restore display
			Win32Context *mContext;
    };
}
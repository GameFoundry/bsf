#pragma once

#include "BsWin32Prerequisites.h"
#include "BsRenderWindow.h"

namespace BansheeEngine 
{
	class Win32Window;

	/**
	 * @brief	Contains various properties that describe a render window.
	 */
	class BS_RSGL_EXPORT Win32RenderWindowProperties : public RenderWindowProperties
	{
	public:
		virtual ~Win32RenderWindowProperties() { }

		/**
		 * @brief	Retrieves the window handle.
		 */
		HWND getHWnd() const { return mHWnd; }

	private:
		friend class Win32WindowCore;
		friend class Win32Window;

		/**
		 * @copydoc	RenderTargetProperties::copyToBuffer
		 */
		virtual void copyToBuffer(UINT8* buffer) const;

		/**
		 * @copydoc	RenderTargetProperties::copyFromBuffer
		 */
		virtual void copyFromBuffer(UINT8* buffer);

		/**
		 * @copydoc	RenderTargetProperties::getSize
		 */
		virtual UINT32 getSize() const;

		HWND mHWnd = 0;
	};

	/**
	 * @brief	Render window implementation for Windows.
	 *
	 * @note	Core thread only.
	 */
    class BS_RSGL_EXPORT Win32WindowCore : public RenderWindowCore
    {
    public:
		Win32WindowCore(Win32Window* parent, RenderWindowProperties* properties, const RENDER_WINDOW_DESC& desc, Win32GLSupport &glsupport);
		~Win32WindowCore();

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
		 * @copydoc RenderWindowCore::copyContentsToMemory
		 */
		void copyToMemory(PixelData &dst, FrameBuffer buffer);

		/**
		 * @copydoc RenderWindowCore::swapBuffers
		 */
		void swapBuffers();

		/**
		 * @copydoc RenderWindowCore::getCustomAttribute
		 */
		void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc RenderWindowCore::setActive
		 */
		virtual void setActive(bool state);

		/**
		 * @copydoc RenderWindowCore::_windowMovedOrResized
		 */
		void _windowMovedOrResized();

		/**
		 * @brief	Returns handle to device context associated with the window.
		 */
		HDC _getHDC() const { return mHDC; }

	protected:
		friend class Win32GLSupport;

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc	CoreObjectCore::destroy
		 */
		virtual void destroy();

		/**
		 * @brief	Calculates window size based on provided client area size and currently set window style. 
		 */
		void getAdjustedWindowSize(UINT32 clientWidth, UINT32 clientHeight, UINT32* winWidth, UINT32* winHeight);

	protected:
		Win32GLSupport &mGLSupport;
		HDC	mHDC;
		DWORD mWindowedStyle;
		DWORD mWindowedStyleEx;
		bool mIsExternal;
		bool mIsChild;
		char* mDeviceName;
		bool mIsExternalGLControl;
		int mDisplayFrequency;
		Win32Context *mContext;
		RENDER_WINDOW_DESC mDesc;
    };

	/**
	 * @brief	Render window implementation for Windows.
	 *
	 * @note	Sim thread only.
	 */
	class BS_RSGL_EXPORT Win32Window : public RenderWindow
	{
	public:
		~Win32Window() { }

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
		 * @copydoc	RenderWindow::getCore
		 */
		Win32WindowCore* getCore() const;

	protected:
		friend class GLRenderWindowManager;
		friend class Win32GLSupport;
		friend class Win32WindowCore;

		Win32Window(Win32GLSupport& glsupport);

		/**
		 * @copydoc	RenderWindow::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderWindow::createCore
		 */
		virtual CoreObjectCore* createCore() const;

	private:
		Win32GLSupport& mGLSupport;
	};
}
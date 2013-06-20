/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#pragma once

#include "CmWin32Prerequisites.h"
#include "CmRenderWindow.h"

namespace CamelotFramework 
{
    class CM_RSGL_EXPORT Win32Window : public RenderWindow
    {
    public:
        ~Win32Window();

		/**
		 * @copydoc RenderWindow::setFullscreen
		 */
		void setFullscreen(bool fullScreen, UINT32 width, UINT32 height);

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
		void reposition(INT32 left, INT32 top);

		/**
		 * @copydoc RenderWindow::resize
		 */
		void resize(UINT32 width, UINT32 height);

		/**
		 * @copydoc RenderWindow::copyContentsToMemory
		 */
		void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer);

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
		Int2 screenToWindowPos(const Int2& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Int2 windowToScreenPos(const Int2& windowPos) const;

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

		void startResize(WindowResizeDirection direction);
		void endResize();

		HWND _getWindowHandle() const { return mHWnd; }
		HDC _getHDC() const { return mHDC; }
		
		void _adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
			unsigned int* winWidth, unsigned int* winHeight);

	protected:
		friend class GLRenderWindowManager;
		friend class Win32GLSupport;

		Win32Window(const RENDER_WINDOW_DESC& desc, Win32GLSupport &glsupport);

		Win32GLSupport &mGLSupport;
		HWND	mHWnd;					// Win32 Window handle
		HDC		mHDC;
		HGLRC	mGlrc;
        bool    mIsExternal;
		char*   mDeviceName;
		bool    mIsExternalGLControl;
		bool	mIsExternalGLContext;
        bool    mSizing;
		bool	mClosed;
        int     mDisplayFrequency;      // fullscreen only, to restore display
        Win32Context *mContext;

		/**
		 * @copydoc RenderWindow::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc RenderWindow::destroy_internal().
		 */
		void destroy_internal();
    };
}
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

#include "CmD3D9Prerequisites.h"
#include "CmRenderWindow.h"
#include "CmD3D9Device.h"

namespace CamelotFramework 
{
	class CM_D3D9_EXPORT D3D9RenderWindow : public RenderWindow
	{
	public:
		~D3D9RenderWindow();
		
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
		bool isClosed() const { return mClosed; }

		/**
		 * @copydoc RenderWindow::isVSync
		 */
		bool isVSync() const { return mVSync; }

		/**
		 * @copydoc RenderWindow::reposition
		 */
		void reposition(INT32 left, INT32 top);

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
		void copyContentsToMemory(const PixelData &dst, FrameBuffer buffer);

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
		Int2 screenToWindowPos(const Int2& screenPos) const;

		/**
		 * @copydoc RenderWindow::windowToScreenPos
		 */
		Int2 windowToScreenPos(const Int2& windowPos) const;

		void startResize(WindowResizeDirection direction);
		void endResize();

		void startMove();
		void endMove();

		/**
		 * @copydoc RenderWindow::_windowMovedOrResized
		 */
		void				_windowMovedOrResized	();

		HWND 				_getWindowHandle		() const { return mHWnd; }				
		IDirect3DDevice9*	_getD3D9Device		() const;
		D3D9Device*			_getDevice			() const;
		void				_setDevice			(D3D9Device* device);

		/**
		 * @brief	Build the presentation parameters used with this window.
		 */
		void				_buildPresentParameters	(D3DPRESENT_PARAMETERS* presentParams) const;

		/**
		 * @brief	Accessor for render surface.
		 */
		IDirect3DSurface9* _getRenderSurface() const;

		/**
		 * @brief	Are we in the middle of switching between fullscreen and windowed.
		 */
		bool _getSwitchingFullscreen() const;

		/**
		 * @brief	Indicate that fullscreen / windowed switching has finished.
		 */
		void _finishSwitchingFullscreen();
	
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
		bool						mSwitchingFullscreen;	// Are we switching from fullscreen to windowed or vice versa		
		D3DMULTISAMPLE_TYPE			mFSAAType;				// AA type.
		DWORD						mFSAAQuality;			// AA quality.
		UINT						mDisplayFrequency;		// Display frequency.
		bool						mVSync;					// Use vertical sync or not.
		unsigned int				mVSyncInterval;		
		DWORD						mStyle;					// Window style currently used for this window.
		bool						mIsDepthBuffered;
		// Desired width / height after resizing
		unsigned int mDesiredWidth;
		unsigned int mDesiredHeight;
	};
}
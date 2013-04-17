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
#ifndef __D3D9RENDERWINDOW_H__
#define __D3D9RENDERWINDOW_H__

#include "CmD3D9Prerequisites.h"
#include "CmRenderWindow.h"
#include "CmD3D9Device.h"

namespace CamelotFramework 
{
	class CM_D3D9_EXPORT D3D9RenderWindow : public RenderWindow
	{
	public:
		~D3D9RenderWindow					();
		
		void				setFullscreen		(bool fullScreen, unsigned int width, unsigned int height);
		bool				isActive			() const;
		bool				isVisible			() const;
		bool 				isClosed			() const { return mClosed; }
		bool				isVSync				() const { return mVSync; }
		void 				reposition			(int left, int top);
		void 				resize				(unsigned int width, unsigned int height);
		HWND 				getWindowHandle		() const { return mHWnd; }				
		IDirect3DDevice9*	getD3D9Device		();
		D3D9Device*			getDevice			();
		void				setDevice			(D3D9Device* device);

		void				getCustomAttribute	(const String& name, void* pData);
		
		/** Overridden - see RenderTarget.
		*/
		void				copyContentsToMemory	(const PixelData &dst, FrameBuffer buffer);
		bool				requiresTextureFlipping	() const { return false; }

		// Method for dealing with resize / move & 3d library
		void				windowMovedOrResized	();
	
		/// Build the presentation parameters used with this window
		void				buildPresentParameters	(D3DPRESENT_PARAMETERS* presentParams);
		
		/// Accessor for render surface
		IDirect3DSurface9* getRenderSurface();

		/// Are we in the middle of switching between fullscreen and windowed
		bool _getSwitchingFullscreen() const;
		
		/// Indicate that fullscreen / windowed switching has finished
		void _finishSwitchingFullscreen();
	
		/// Returns true if this window use depth buffer.
		bool isDepthBuffered() const;

		/// Returns true if this window should use NV perf hud adapter.
		bool isNvPerfHUDEnable() const;

		/** Validate the device for this window. */
		bool _validateDevice();

		void adjustWindow(unsigned int clientWidth, unsigned int clientHeight, 
			DWORD style, unsigned int* winWidth, unsigned int* winHeight);

	protected:
		friend class D3D9RenderWindowManager;

		D3D9RenderWindow (const RENDER_WINDOW_DESC& desc, HINSTANCE instance);

		void updateWindowRect();

		void swapBuffers_internal();

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
		bool						mUseNVPerfHUD;			// Use NV Perf HUD.
		DWORD						mStyle;					// Window style currently used for this window.
		bool						mIsDepthBuffered;
		// Desired width / height after resizing
		unsigned int mDesiredWidth;
		unsigned int mDesiredHeight;
	};
}
#endif

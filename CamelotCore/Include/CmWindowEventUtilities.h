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
#ifndef __OgreWindowEventUtils_H__
#define __OgreWindowEventUtils_H__

#include "CmPrerequisites.h"
#include <boost/signals.hpp>

#if CM_PLATFORM == CM_PLATFORM_WIN32
#  define WIN32_LEAN_AND_MEAN
#  if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#  endif
#  include <windows.h>
#  include <windowsx.h>
#elif CM_PLATFORM == CM_PLATFORM_APPLE && !defined(__LP64__)
#  include <Carbon/Carbon.h>
#endif

#define WM_CM_HIDECURSOR WM_USER + 101
#define WM_CM_SHOWCURSOR WM_USER + 102

namespace CamelotFramework
{
	/**
	@Remarks
		Utility class to handle Window Events/Pumping/Messages
	*/
	class CM_EXPORT WindowEventUtilities
	{
	public:
		/**
		@Remarks
			Call this once per frame if not using Root:startRendering(). This will update all registered
			RenderWindows (If using external Windows, you can optionally register those yourself)
		*/
		static void messagePump();

		/**
		@Remarks
			Called by RenderWindows upon creation for Ogre generated windows. You are free to add your
			external windows here too if needed.
		@param window
			The RenderWindow to monitor
		*/
		static void _addRenderWindow(RenderWindow* window);

		/**
		@Remarks
			Called by RenderWindows upon creation for Ogre generated windows. You are free to add your
			external windows here too if needed.
		@param window
			The RenderWindow to remove from list
		*/
		static void _removeRenderWindow(RenderWindow* window);

#if CM_PLATFORM == CM_PLATFORM_WIN32
		//! Internal winProc (RenderWindow's use this when creating the Win32 Window)
		static LRESULT CALLBACK _WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
#elif CM_PLATFORM == CM_PLATFORM_APPLE && !defined __OBJC__ && !defined(__LP64__)
        //! Internal UPP Window Handler (RenderWindow's use this when creating the OS X Carbon Window
        static OSStatus _CarbonWindowHandler(EventHandlerCallRef nextHandler, EventRef event, void* wnd);
#endif

		typedef Vector<RenderWindow*>::type Windows;
		static Windows _msWindows;

		static boost::signal<void(const Int2&)> onMouseMoved;
		static boost::signal<void(UINT32)> onCharInput;
	};
	/** @} */
	/** @} */
}
#endif

#include "X11/CmOSCursorImpl.h"

// TODO - Not tested and will not work until I properly test it on a unix system!

namespace CamelotEngine
{
	OSCursor()
		:mCursorGrabbed(false), mCursorHidden(false)
	{
		mDisplay = XOpenDisplay(0); // TODO - What if there are multiple displays? Need to check for that
		mWindow = XRootWindow(mDisplay, 0);
		mCursor = createNULLCursor(mDisplay, mWindow); // Create invisible cursor
	}

	Int2 OSCursor::getPosition()
	{
		Window returnedWindow;
		int rootX, rootY;
		int winX, winY;
		unsigned int returnedMask;

		XQueryPointer(mDisplay, mWindow, &returnedWindow,
			&returnedWindow, &rootX, &rootY, &winX, &winY,
			&returnedMask)

		return Int2(rootX, rootY);
	}

	void OSCursor::setPosition(const Int2& pos)
	{
		XWarpPointer(mDisplay, None, mWindow, 0,0,0,0, pos.x, pos.y);
		XFlush(mDisplay);
	}

	void OSCursor::hide()
	{
		// Hide cursor
		if(!mCursorHidden)
		{
			XDefineCursor(mDisplay, mWindow, mCursor );
			mCursorHidden = true;
		}

		// Grab cursor to user window
		if(!mCursorGrabbed)
		{
			if( XGrabPointer( mDisplay, mWindow, True,
				ButtonPressMask | ButtonReleaseMask |
				PointerMotionMask, GrabModeAsync, GrabModeAsync,
				mWindow, None, CurrentTime ) ==
				GrabSuccess )
			{
				mCursorGrabbed = true;
			}
		}
	}

	void OSCursor::show()
	{
		// Un-grab cursor (only in windowed mode: in fullscreen mode we still
		// want the mouse grabbed in order to confine the cursor to the window
		// area)
		if(mCursorGrabbed)
		{
			XUngrabPointer(mDisplay, CurrentTime);
			mCursorGrabbed = false;
		}

		// Show cursor
		if(mCursorHidden)
		{
			XUndefineCursor(mDisplay, mWindow);
			mCursorHidden = GL_FALSE;
		}
	}
}
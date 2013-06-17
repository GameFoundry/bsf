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
#include "CmRenderWindow.h"
#include "CmCoreThread.h"
#include "CmRenderWindowManager.h"
#include "CmViewport.h"

namespace CamelotFramework 
{
    RenderWindow::RenderWindow(const RENDER_WINDOW_DESC& desc)
        : RenderTarget()
		, mIsFullScreen(false)
		, mDesc(desc)
		, mHasFocus(false)
    {

    }

	RenderWindow::~RenderWindow() 
	{
		
	}

    void RenderWindow::getMetrics(UINT32& width, UINT32& height, UINT32& colourDepth,
		INT32& left, INT32& top)
    {
        width = mWidth;
        height = mHeight;
        colourDepth = mColorDepth;
        left = mLeft;
        top = mTop;
    }

	void RenderWindow::setVisible(bool visible)
	{
		THROW_IF_NOT_CORE_THREAD;
	}

    bool RenderWindow::isFullScreen(void) const
    {
        return mIsFullScreen;
    }

	void RenderWindow::_windowMovedOrResized()
	{
		THROW_IF_NOT_CORE_THREAD;

		if(!onMovedOrResized.empty())
			onMovedOrResized(this);

		RenderWindowManager::instance().windowMovedOrResized(this);
	}

	void RenderWindow::_windowFocusReceived()
	{
		THROW_IF_NOT_CORE_THREAD;

		mHasFocus = true;

		RenderWindowManager::instance().windowFocusReceived(this);
	}

	void RenderWindow::_windowFocusLost()
	{
		THROW_IF_NOT_CORE_THREAD;

		mHasFocus = false;
	}

	void RenderWindow::destroy()
	{
		RenderWindowManager::instance().windowDestroyed(this);

		RenderTarget::destroy();
	}

	RenderWindowPtr RenderWindow::create(RENDER_WINDOW_DESC& desc, RenderWindowPtr parentWindow)
	{
		return RenderWindowManager::instance().create(desc, parentWindow);
	}
}

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
#include "CmRenderTarget.h"

#include "CmViewport.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotEngine {

    RenderTarget::RenderTarget()
		:mPriority(CM_DEFAULT_RT_GROUP),
		mActive(true),
		mHwGamma(false), 
		mFSAA(0)
    {
    }

    RenderTarget::~RenderTarget()
    {
    }

	void RenderTarget::destroy()
	{
		RenderSystem::instancePtr()->queueCommand(boost::bind(&RenderTarget::destroy_internal, this));
	}

	void RenderTarget::destroy_internal()
	{
		IDestroyable::destroy();
	}

    const String& RenderTarget::getName(void) const
    {
        return mName;
    }


    void RenderTarget::getMetrics(unsigned int& width, unsigned int& height, unsigned int& colourDepth)
    {
        width = mWidth;
        height = mHeight;
        colourDepth = mColorDepth;
    }

    unsigned int RenderTarget::getWidth(void) const
    {
        return mWidth;
    }
    unsigned int RenderTarget::getHeight(void) const
    {
        return mHeight;
    }
    unsigned int RenderTarget::getColourDepth(void) const
    {
        return mColorDepth;
    }


	void RenderTarget::_beginUpdate()
	{
	}

	void RenderTarget::_endUpdate()
	{
	}

	void RenderTarget::getCustomAttribute(const String& name, void* pData)
    {
        CM_EXCEPT(InvalidParametersException, "Attribute not found.");
    }
    //-----------------------------------------------------------------------
    bool RenderTarget::isActive() const
    {
        return mActive;
    }
    //-----------------------------------------------------------------------
    void RenderTarget::setActive( bool state )
    {
        mActive = state;
    }
    //-----------------------------------------------------------------------
    bool RenderTarget::isPrimary(void) const
    {
        // RenderWindow will override and return true for the primary window
        return false;
    }
    //-----------------------------------------------------------------------
    void RenderTarget::update(bool swap)
    {
		if (swap)
		{
			// Swap buffers
    	    swapBuffers(CamelotEngine::RenderSystem::instancePtr()->getWaitForVerticalBlank());
		}
    }
	

}        

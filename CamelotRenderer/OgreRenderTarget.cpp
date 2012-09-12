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
#include "OgreRenderTarget.h"
#include "OgreStringConverter.h"

#include "OgreViewport.h"
#include "OgreException.h"
#include "OgreRenderSystem.h"
#include "CmRenderSystemManager.h"

namespace CamelotEngine {

    RenderTarget::RenderTarget()
		:mPriority(OGRE_DEFAULT_RT_GROUP),
		mActive(true),
		mAutoUpdate(true),
		mHwGamma(false), 
		mFSAA(0)
    {
    }

    RenderTarget::~RenderTarget()
    {
        // Delete viewports
        for (ViewportList::iterator i = mViewportList.begin();
            i != mViewportList.end(); ++i)
        {
            delete (*i).second;
        }
    }

    const String& RenderTarget::getName(void) const
    {
        return mName;
    }


    void RenderTarget::getMetrics(unsigned int& width, unsigned int& height, unsigned int& colourDepth)
    {
        width = mWidth;
        height = mHeight;
        colourDepth = mColourDepth;
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
        return mColourDepth;
    }

    void RenderTarget::updateImpl(void)
    {
		_beginUpdate();
		_endUpdate();
    }

	void RenderTarget::_beginUpdate()
	{
	}

	void RenderTarget::_endUpdate()
	{
	}

	void RenderTarget::_updateViewport(Viewport* viewport, bool updateStatistics)
	{
		assert(viewport->getTarget() == this &&
				"RenderTarget::_updateViewport the requested viewport is "
				"not bound to the rendertarget!");

		viewport->update();
	}

	void RenderTarget::_updateViewport(int zorder, bool updateStatistics)
	{
		ViewportList::iterator it = mViewportList.find(zorder);
        if (it != mViewportList.end())
        {
			_updateViewport((*it).second,updateStatistics);
		}
		else
		{
			OGRE_EXCEPT(Exception::ERR_ITEM_NOT_FOUND,"No viewport with given zorder : "
				+ StringConverter::toString(zorder), "RenderTarget::_updateViewport");
		}
	}

    Viewport* RenderTarget::addViewport(int ZOrder, float left, float top ,
        float width , float height)
    {
        // Check no existing viewport with this Z-order
        ViewportList::iterator it = mViewportList.find(ZOrder);

        if (it != mViewportList.end())
        {
			StringUtil::StrStreamType str;
			str << "Can't create another viewport for "
				<< mName << " with Z-Order " << ZOrder
				<< " because a viewport exists with this Z-Order already.";
			OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, str.str(), "RenderTarget::addViewport");
        }
        // Add viewport to list
        // Order based on Z-Order
        Viewport* vp = new Viewport(this, left, top, width, height, ZOrder);

        mViewportList.insert(ViewportList::value_type(ZOrder, vp));

        return vp;
    }
	//-----------------------------------------------------------------------
    void RenderTarget::removeViewport(int ZOrder)
    {
        ViewportList::iterator it = mViewportList.find(ZOrder);

        if (it != mViewportList.end())
        {
            delete (*it).second;
            mViewportList.erase(ZOrder);
        }
    }

    void RenderTarget::removeAllViewports(void)
    {
        for (ViewportList::iterator it = mViewportList.begin(); it != mViewportList.end(); ++it)
        {
            delete (*it).second;
        }

        mViewportList.clear();

    }

    void RenderTarget::getCustomAttribute(const String& name, void* pData)
    {
        OGRE_EXCEPT(Exception::ERR_INVALIDPARAMS, "Attribute not found.", "RenderTarget::getCustomAttribute");
    }
    //-----------------------------------------------------------------------
    unsigned short RenderTarget::getNumViewports(void) const
    {
        return (unsigned short)mViewportList.size();

    }
    //-----------------------------------------------------------------------
    Viewport* RenderTarget::getViewport(unsigned short index)
    {
        assert (index < mViewportList.size() && "Index out of bounds");

        ViewportList::iterator i = mViewportList.begin();
        while (index--)
            ++i;
        return i->second;
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
    RenderTarget::Impl *RenderTarget::_getImpl()
    {
        return 0;
    }
    //-----------------------------------------------------------------------
    void RenderTarget::update(bool swap)
    {
        // call implementation
        updateImpl();


		if (swap)
		{
			// Swap buffers
    	    swapBuffers(CamelotEngine::RenderSystemManager::getActive()->getWaitForVerticalBlank());
		}
    }
	

}        

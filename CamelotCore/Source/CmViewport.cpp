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
#include "CmViewport.h"

#include "CmException.h"
#include "CmRenderTarget.h"
#include "CmMath.h"
#include "CmRenderSystem.h"

namespace CamelotFramework 
{
	Viewport::Viewport()
		:mTarget(nullptr)
		, mRelLeft(0)
		, mRelTop(0)
		, mRelWidth(0)
		, mRelHeight(0)
		// Actual dimensions will update later
		, mZOrder(0)
	{
		// Calculate actual dimensions
		updateDimensions();
	}

    Viewport::Viewport(RenderTargetPtr target, float left, float top, float width, float height, int ZOrder)
         :mTarget(target)
        , mRelLeft(left)
        , mRelTop(top)
        , mRelWidth(width)
        , mRelHeight(height)
        // Actual dimensions will update later
        , mZOrder(ZOrder)
    {
		if(target != nullptr)
		{
			mTargetConn = target->onMovedOrResized.connect(boost::bind(&Viewport::targetResized, this, _1));
		}

        // Calculate actual dimensions
        updateDimensions();
    }

    Viewport::~Viewport()
    {
		mTargetConn.disconnect();
    }

	void Viewport::targetResized(RenderTarget* target)
	{
		updateDimensions();
	}

    void Viewport::updateDimensions(void)
    {
		if(mTarget != nullptr)
		{
			float height = (float) mTarget->getHeight();
			float width = (float) mTarget->getWidth();

			mDimensions.x = (int) (mRelLeft * width);
			mDimensions.y = (int) (mRelTop * height);
			mDimensions.width = (int) (mRelWidth * width);
			mDimensions.height = (int) (mRelHeight * height);
		}
    }

	int Viewport::getZOrder(void) const
	{
		return mZOrder;
	}

    RenderTargetPtr Viewport::getTarget(void) const
    {
        return mTarget;
    }

    float Viewport::getNormalizedLeft(void) const
    {
        return mRelLeft;
    }

    float Viewport::getNormalizedTop(void) const
    {
        return mRelTop;
    }

    float Viewport::getNormalizedWidth(void) const
    {
        return mRelWidth;
    }

    float Viewport::getNormalizedHeight(void) const
    {
        return mRelHeight;
    }

    int Viewport::getLeft(void) const
    {
        return mDimensions.x;
    }

    int Viewport::getTop(void) const
    {
        return mDimensions.y;
    }

    int Viewport::getWidth(void) const
    {
        return mDimensions.width;
    }

    int Viewport::getHeight(void) const
    {
        return mDimensions.height;
    }

    void Viewport::setDimensions(float left, float top, float width, float height)
    {
        mRelLeft = left;
        mRelTop = top;
        mRelWidth = width;
        mRelHeight = height;
        updateDimensions();
    }
}

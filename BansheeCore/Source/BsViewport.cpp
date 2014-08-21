#include "BsViewport.h"

#include "BsException.h"
#include "BsRenderTarget.h"
#include "BsMath.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
	const Color Viewport::DEFAULT_CLEAR_COLOR = Color(143.0f / 255.0f, 111.0f / 255.0f, 0);

	Viewport::Viewport()
		:mTarget(nullptr), mClearColor(DEFAULT_CLEAR_COLOR), mRequiresColorClear(true), mRequiresDepthClear(true), 
		mRequiresStencilClear(false), mStencilClearValue(0), mDepthClearValue(1.0f), mCoreDirtyFlags(0xFFFFFFFF)
	{

	}

    Viewport::Viewport(const RenderTargetPtr& target, float x, float y, float width, float height)
         :mTarget(target), mNormArea(x, y, width, height), mClearColor(DEFAULT_CLEAR_COLOR), mRequiresColorClear(true), 
		 mRequiresDepthClear(true), mRequiresStencilClear(false), mStencilClearValue(0), mDepthClearValue(1.0f), mCoreDirtyFlags(0xFFFFFFFF)
    {

    }

    Viewport::~Viewport()
    { }


    void Viewport::setArea(float x, float y, float width, float height)
    {
        mNormArea.x = x;
        mNormArea.y = y;
        mNormArea.width = width;
        mNormArea.height = height;

		markCoreDirty();
    }

	RectI Viewport::getArea() const
	{
		float width = (float)mTarget->getWidth();
		float height = (float)mTarget->getHeight();
		
		RectI area;
		area.x = (int)(mNormArea.x * width);
		area.y = (int)(mNormArea.y * height);
		area.width = (int)(mNormArea.width * width);
		area.height = (int)(mNormArea.height * height);

		return area;
	}

	void Viewport::setRequiresClear(bool colorClear, bool depthClear, bool stencilClear)
	{
		mRequiresColorClear = colorClear;
		mRequiresDepthClear = depthClear;
		mRequiresStencilClear = stencilClear;

		markCoreDirty();
	}

	void Viewport::setClearValues(const Color& clearColor, float clearDepth, UINT16 clearStencil)
	{
		mClearColor = clearColor;
		mDepthClearValue = clearDepth;
		mStencilClearValue = clearStencil;

		markCoreDirty();
	}

	INT32 Viewport::getX() const 
	{ 
		return (INT32)(mNormArea.x * mTarget->getWidth());
	}

	INT32 Viewport::getY() const 
	{ 
		return (INT32)(mNormArea.y * mTarget->getHeight());
	}

	INT32 Viewport::getWidth() const 
	{ 
		return (INT32)(mNormArea.width * mTarget->getWidth());
	}

	INT32 Viewport::getHeight() const 
	{ 
		return (INT32)(mNormArea.height * mTarget->getHeight());
	}

	Viewport Viewport::clone()
	{
		return *this;
	}
}

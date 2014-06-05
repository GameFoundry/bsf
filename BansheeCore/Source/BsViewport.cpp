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
		mRequiresStencilClear(false), mStencilClearValue(0), mDepthClearValue(1.0f)
	{
		updateArea();
	}

    Viewport::Viewport(const RenderTargetPtr& target, float x, float y, float width, float height)
         :mTarget(target), mNormArea(x, y, width, height), mClearColor(DEFAULT_CLEAR_COLOR), mRequiresColorClear(true), 
		 mRequiresDepthClear(true), mRequiresStencilClear(false), mStencilClearValue(0), mDepthClearValue(1.0f)
    {
        updateArea();
    }

    Viewport::~Viewport()
    { }

	void Viewport::targetResized()
	{
		updateArea();
	}

    void Viewport::updateArea()
    {
		if(mTarget != nullptr)
		{
			float height = (float) mTarget->getHeight();
			float width = (float) mTarget->getWidth();

			mArea.x = (int) (mNormArea.x * width);
			mArea.y = (int) (mNormArea.y * height);
			mArea.width = (int) (mNormArea.width * width);
			mArea.height = (int) (mNormArea.height * height);
		}
    }

    void Viewport::setArea(float x, float y, float width, float height)
    {
        mNormArea.x = x;
        mNormArea.y = y;
        mNormArea.width = width;
        mNormArea.height = height;

        updateArea();
    }

	Viewport Viewport::clone()
	{
		return *this;
	}
}

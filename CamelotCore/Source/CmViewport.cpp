#include "CmViewport.h"

#include "CmException.h"
#include "CmRenderTarget.h"
#include "CmMath.h"
#include "CmRenderSystem.h"

namespace CamelotFramework 
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
		if(target != nullptr)
		{
			// Note: RenderTarget resize will only get triggered for RenderWindows, RenderTextures are immutable
			mTargetResizedConn = target->onResized.connect(boost::bind(&Viewport::targetResized, this));
		}

        updateArea();
    }

    Viewport::~Viewport()
    {
		mTargetResizedConn.disconnect();
    }

	void Viewport::targetResized()
	{
		updateArea();

		if(!onResized.empty())
			onResized();
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
}

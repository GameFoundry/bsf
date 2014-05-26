#include "CmRenderTarget.h"
#include "CmViewport.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace BansheeEngine 
{
    RenderTarget::RenderTarget()
		:mActive(true), mHwGamma(false), mVSync(false), mMultisampleCount(0),
		mWidth(0), mHeight(0), mColorDepth(0), mPriority(0)
    {
    }

    RenderTarget::~RenderTarget()
    {
    }

	void RenderTarget::getCustomAttribute(const String& name, void* pData) const
    {
        CM_EXCEPT(InvalidParametersException, "Attribute not found.");
    }
}        

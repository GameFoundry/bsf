#include "BsRenderTarget.h"
#include "BsViewport.h"
#include "BsException.h"
#include "BsRenderSystem.h"

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
        BS_EXCEPT(InvalidParametersException, "Attribute not found.");
    }
}        

#include "CmSamplerState.h"
#include "CmSamplerStateRTTI.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmException.h"

namespace CamelotEngine 
{
	void SamplerState::initialize(const SAMPLER_STATE_DESC& desc)
	{
		mData = desc;

		Resource::initialize();
	}

	const SamplerStatePtr& SamplerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultSamplerState();
	}

	FilterOptions SamplerState::getTextureFiltering(FilterType ft) const
	{
        switch (ft)
        {
        case FT_MIN:
            return mData.minFilter;
        case FT_MAG:
            return mData.magFilter;
        case FT_MIP:
            return mData.mipFilter;
        }

		return mData.minFilter;
	}

	const Color& SamplerState::getBorderColor() const
	{
		return mData.borderColor;
	}

	SamplerStateHandle SamplerState::create(const SAMPLER_STATE_DESC& desc)
	{
		SamplerStatePtr samplerPtr = RenderStateManager::instance().createSamplerState(desc);

		return static_resource_cast<SamplerState>(Resource::_createResourceHandle(samplerPtr));
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* SamplerState::getRTTIStatic()
	{
		return SamplerStateRTTI::instance();
	}

	RTTITypeBase* SamplerState::getRTTI() const
	{
		return SamplerState::getRTTIStatic();
	}
}

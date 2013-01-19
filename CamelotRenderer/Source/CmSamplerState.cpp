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

		RenderSystem::instancePtr()->queueCommand(boost::bind(&SamplerState::initialize_internal, this));
	}

	const SamplerStatePtr& SamplerState::getDefault()
	{
		static SamplerStatePtr samplerState = RenderStateManager::instance().createSamplerState(SAMPLER_STATE_DESC());

		return samplerState;
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

	SamplerStatePtr SamplerState::create(const SAMPLER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createSamplerState(desc);
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

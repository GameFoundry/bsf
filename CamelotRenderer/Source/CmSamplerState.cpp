#include "CmSamplerState.h"
#include "CmSamplerStateRTTI.h"
#include "CmRenderStateManager.h"
#include "CmException.h"

namespace CamelotEngine 
{
	void SamplerState::initialize(const SAMPLER_STATE_DESC& desc)
	{
		mData = desc;
	}

	const SamplerState& SamplerState::getDefault()
	{
		static SamplerState samplerState;
		static bool initialized = false;

		if(!initialized)
		{
			samplerState.initialize(SAMPLER_STATE_DESC());
			initialized = true;
		}

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

	const Color& SamplerState::getBorderColor(UINT32 idx) const
	{
		assert(idx >= 0 && idx < 4);

		return mData.borderColor[idx];
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

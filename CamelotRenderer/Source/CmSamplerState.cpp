#include "CmSamplerState.h"
#include "CmSamplerStateRTTI.h"
#include "CmRenderStateManager.h"
#include "CmException.h"

namespace CamelotEngine 
{
	SamplerState SamplerState::DEFAULT;

	void SamplerState::initialize(const SAMPLER_STATE_DESC& desc)
	{
		mData = desc;
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

	const Color& SamplerState::getBorderColor(UINT32 idx)
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

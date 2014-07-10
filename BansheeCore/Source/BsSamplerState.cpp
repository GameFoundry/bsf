//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsSamplerState.h"
#include "BsSamplerStateRTTI.h"
#include "BsRenderStateManager.h"
#include "BsRenderSystem.h"
#include "BsException.h"
#include "BsResources.h"

namespace BansheeEngine 
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

	HSamplerState SamplerState::create(const SAMPLER_STATE_DESC& desc)
	{
		SamplerStatePtr samplerPtr = RenderStateManager::instance().createSamplerState(desc);

		return static_resource_cast<SamplerState>(gResources()._createResourceHandle(samplerPtr));
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

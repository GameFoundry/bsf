#include "BsSamplerState.h"
#include "BsSamplerStateRTTI.h"
#include "BsRenderStateManager.h"
#include "BsRenderAPI.h"
#include "BsException.h"
#include "BsResources.h"

namespace BansheeEngine 
{
	SamplerProperties::SamplerProperties(const SAMPLER_STATE_DESC& desc)
		:mData(desc)
	{ }

	FilterOptions SamplerProperties::getTextureFiltering(FilterType ft) const
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

	const Color& SamplerProperties::getBorderColor() const
	{
		return mData.borderColor;
	}

	SamplerStateCore::SamplerStateCore(const SAMPLER_STATE_DESC& desc)
		:mProperties(desc)
	{

	}

	const SamplerProperties& SamplerStateCore::getProperties() const
	{
		return mProperties;
	}

	const SPtr<SamplerStateCore>& SamplerStateCore::getDefault()
	{
		return RenderStateCoreManager::instance().getDefaultSamplerState();
	}

	SamplerState::SamplerState(const SAMPLER_STATE_DESC& desc)
		:mProperties(desc)
	{

	}

	SPtr<SamplerStateCore> SamplerState::getCore() const
	{
		return std::static_pointer_cast<SamplerStateCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> SamplerState::createCore() const
	{
		return RenderStateCoreManager::instance().createSamplerStateInternal(mProperties.mData);
	}

	HSamplerState SamplerState::create(const SAMPLER_STATE_DESC& desc)
	{
		SamplerStatePtr samplerPtr = RenderStateManager::instance().createSamplerState(desc);

		return static_resource_cast<SamplerState>(gResources()._createResourceHandle(samplerPtr));
	}

	const SamplerStatePtr& SamplerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultSamplerState();
	}

	const SamplerProperties& SamplerState::getProperties() const
	{
		return mProperties;
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

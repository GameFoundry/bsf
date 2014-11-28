#include "BsRasterizerState.h"
#include "BsRenderStateManager.h"
#include "BsRenderAPI.h"
#include "BsRasterizerStateRTTI.h"
#include "BsResources.h"

namespace BansheeEngine
{
	RasterizerProperties::RasterizerProperties(const RASTERIZER_STATE_DESC& desc)
		:mData(desc)
	{ }

	RasterizerStateCore::RasterizerStateCore(const RASTERIZER_STATE_DESC& desc)
		: mProperties(desc)
	{

	}

	const RasterizerProperties& RasterizerStateCore::getProperties() const
	{
		return mProperties;
	}

	const SPtr<RasterizerStateCore>& RasterizerStateCore::getDefault()
	{
		return RenderStateCoreManager::instance().getDefaultRasterizerState();
	}

	RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc)
		: mProperties(desc)
	{

	}

	SPtr<RasterizerStateCore> RasterizerState::getCore() const
	{
		return std::static_pointer_cast<RasterizerStateCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> RasterizerState::createCore() const
	{
		return RenderStateCoreManager::instance().createRasterizerStateInternal(mProperties.mData);
	}

	const RasterizerProperties& RasterizerState::getProperties() const
	{
		return mProperties;
	}

	const RasterizerStatePtr& RasterizerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultRasterizerState();
	}

	HRasterizerState RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		RasterizerStatePtr rasterizerPtr = RenderStateManager::instance().createRasterizerState(desc);

		return static_resource_cast<RasterizerState>(gResources()._createResourceHandle(rasterizerPtr));
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* RasterizerState::getRTTIStatic()
	{
		return RasterizerStateRTTI::instance();
	}

	RTTITypeBase* RasterizerState::getRTTI() const
	{
		return RasterizerState::getRTTIStatic();
	}
}
#include "CmRasterizerState.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmRasterizerStateRTTI.h"

namespace CamelotEngine
{
	void RasterizerState::initialize(const RASTERIZER_STATE_DESC& desc)
	{
		mData = desc;

		Resource::initialize();
	}

	const RasterizerStatePtr& RasterizerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultRasterizerState();
	}

	RasterizerStateHandle RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		RasterizerStatePtr rasterizerPtr = RenderStateManager::instance().createRasterizerState(desc);

		return static_resource_cast<RasterizerState>(Resource::_createResourceHandle(rasterizerPtr));
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
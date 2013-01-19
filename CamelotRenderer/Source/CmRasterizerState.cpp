#include "CmRasterizerState.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmRasterizerStateRTTI.h"

namespace CamelotEngine
{
	void RasterizerState::initialize(const RASTERIZER_STATE_DESC& desc)
	{
		mData = desc;

		RenderSystem::instancePtr()->queueCommand(boost::bind(&RasterizerState::initialize_internal, this));
	}

	const RasterizerStatePtr& RasterizerState::getDefault()
	{
		static RasterizerStatePtr rasterizerState = RenderStateManager::instance().createRasterizerState(RASTERIZER_STATE_DESC());

		return rasterizerState;
	}

	RasterizerStatePtr RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createRasterizerState(desc);
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
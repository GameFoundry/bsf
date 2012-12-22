#include "CmRasterizerState.h"
#include "CmRenderStateManager.h"
#include "CmRasterizerStateRTTI.h"

namespace CamelotEngine
{
	void RasterizerState::initialize(const RASTERIZER_STATE_DESC& desc)
	{
		mData = desc;
	}

	const RasterizerState& RasterizerState::getDefault()
	{
		static RasterizerState rasterizerState;
		static bool initialized = false;

		if(!initialized)
		{
			rasterizerState.initialize(RASTERIZER_STATE_DESC());
			initialized = true;
		}

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
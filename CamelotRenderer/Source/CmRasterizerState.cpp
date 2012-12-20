#include "CmRasterizerState.h"
#include "CmRenderStateManager.h"

namespace CamelotEngine
{
	void RasterizerState::initialize(const RASTERIZER_STATE_DESC& desc)
	{
		mData = desc;
	}

	RasterizerStatePtr RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createRasterizerState(desc);
	}
}
#include "CmDepthStencilState.h"
#include "CmRenderStateManager.h"
#include "CmException.h"

namespace CamelotEngine
{
	void DepthStencilState::initialize(const DEPTH_STENCIL_DESC& desc)
	{
		mData = desc;
		mStencilRefValue = 0;
	}

	DepthStencilState::DepthStencilState()
		:mStencilRefValue(0)
	{

	}

	DepthStencilStatePtr DepthStencilState::create(const DEPTH_STENCIL_DESC& desc)
	{
		return RenderStateManager::instance().createDepthStencilState(desc);
	}
}
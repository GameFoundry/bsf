#include "CmRenderStateManager.h"
#include "CmSamplerState.h"
#include "CmDepthStencilState.h"

namespace CamelotEngine
{
	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(new SamplerState());
		samplerState->initialize(desc);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(new DepthStencilState());
		depthStencilState->initialize(desc);

		return depthStencilState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		return SamplerStatePtr(new SamplerState());
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		return DepthStencilStatePtr(new DepthStencilState());
	}
}
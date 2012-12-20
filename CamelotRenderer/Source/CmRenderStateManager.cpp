#include "CmRenderStateManager.h"
#include "CmSamplerState.h"
#include "CmDepthStencilState.h"
#include "CmRasterizerState.h"
#include "CmBlendState.h"

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

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(new RasterizerState());
		rasterizerState->initialize(desc);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = BlendStatePtr(new BlendState());
		blendState->initialize(desc);

		return blendState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		return SamplerStatePtr(new SamplerState());
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		return DepthStencilStatePtr(new DepthStencilState());
	}

	RasterizerStatePtr RenderStateManager::createEmptyRasterizerState() const
	{
		return RasterizerStatePtr(new RasterizerState());
	}

	BlendStatePtr RenderStateManager::createEmptyBlendState() const
	{
		return BlendStatePtr(new BlendState());
	}
}
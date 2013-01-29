#include "CmRenderStateManager.h"
#include "CmSamplerState.h"
#include "CmDepthStencilState.h"
#include "CmRasterizerState.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	RenderStateManager::RenderStateManager()
	{
		mDefaultSamplerState = createSamplerState(SAMPLER_STATE_DESC());
		mDefaultBlendState = createBlendState(BLEND_STATE_DESC());
		mDefaultRasterizerState = createRasterizerState(RASTERIZER_STATE_DESC());
		mDefaultDepthStencilState = createDepthStencilState(DEPTH_STENCIL_STATE_DESC());
	}

	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(new SamplerState());
		samplerState->setThisPtr(samplerState);
		samplerState->initialize(desc);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(new DepthStencilState());
		depthStencilState->setThisPtr(depthStencilState);
		depthStencilState->initialize(desc);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(new RasterizerState());
		rasterizerState->setThisPtr(rasterizerState);
		rasterizerState->initialize(desc);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = BlendStatePtr(new BlendState());
		blendState->setThisPtr(blendState);
		blendState->initialize(desc);

		return blendState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(new SamplerState());
		samplerState->setThisPtr(samplerState);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(new DepthStencilState());
		depthStencilState->setThisPtr(depthStencilState);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createEmptyRasterizerState() const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(new RasterizerState());
		rasterizerState->setThisPtr(rasterizerState);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createEmptyBlendState() const
	{
		BlendStatePtr blendState = BlendStatePtr(new BlendState());
		blendState->setThisPtr(blendState);

		return blendState;
	}
}
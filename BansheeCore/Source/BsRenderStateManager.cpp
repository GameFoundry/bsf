#include "BsRenderStateManager.h"
#include "BsSamplerState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"

namespace BansheeEngine
{
	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = createSamplerStateImpl();
		samplerState->_setThisPtr(samplerState);
		samplerState->initialize(desc);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = createDepthStencilStateImpl();
		depthStencilState->_setThisPtr(depthStencilState);
		depthStencilState->initialize(desc);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = createRasterizerStateImpl();
		rasterizerState->_setThisPtr(rasterizerState);
		rasterizerState->initialize(desc);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = createBlendStateImpl();
		blendState->_setThisPtr(blendState);
		blendState->initialize(desc);

		return blendState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		SamplerStatePtr samplerState = createSamplerStateImpl();
		samplerState->_setThisPtr(samplerState);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		DepthStencilStatePtr depthStencilState = createDepthStencilStateImpl();
		depthStencilState->_setThisPtr(depthStencilState);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createEmptyRasterizerState() const
	{
		RasterizerStatePtr rasterizerState = createRasterizerStateImpl();
		rasterizerState->_setThisPtr(rasterizerState);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createEmptyBlendState() const
	{
		BlendStatePtr blendState = createBlendStateImpl();
		blendState->_setThisPtr(blendState);

		return blendState;
	}

	const SamplerStatePtr& RenderStateManager::getDefaultSamplerState() const 
	{ 
		if(mDefaultSamplerState == nullptr)
			mDefaultSamplerState = createSamplerState(SAMPLER_STATE_DESC());

		return mDefaultSamplerState; 
	}

	const BlendStatePtr& RenderStateManager::getDefaultBlendState() const 
	{ 
		if(mDefaultBlendState == nullptr)
			mDefaultBlendState = createBlendState(BLEND_STATE_DESC());

		return mDefaultBlendState; 
	}

	const RasterizerStatePtr& RenderStateManager::getDefaultRasterizerState() const 
	{ 
		if(mDefaultRasterizerState == nullptr)
			mDefaultRasterizerState = createRasterizerState(RASTERIZER_STATE_DESC());

		return mDefaultRasterizerState; 
	}

	const DepthStencilStatePtr& RenderStateManager::getDefaultDepthStencilState() const 
	{ 
		if(mDefaultDepthStencilState == nullptr)
			mDefaultDepthStencilState = createDepthStencilState(DEPTH_STENCIL_STATE_DESC());

		return mDefaultDepthStencilState; 
	}

	SamplerStatePtr RenderStateManager::createSamplerStateImpl() const
	{
		return bs_core_ptr<SamplerState, PoolAlloc>();
	}

	BlendStatePtr RenderStateManager::createBlendStateImpl() const
	{
		return bs_core_ptr<BlendState, PoolAlloc>();
	}

	RasterizerStatePtr RenderStateManager::createRasterizerStateImpl() const
	{
		return bs_core_ptr<RasterizerState, PoolAlloc>();
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilStateImpl() const
	{
		return bs_core_ptr<DepthStencilState, PoolAlloc>();
	}
}
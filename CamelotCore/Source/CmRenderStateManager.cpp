#include "CmRenderStateManager.h"
#include "CmSamplerState.h"
#include "CmDepthStencilState.h"
#include "CmRasterizerState.h"
#include "CmBlendState.h"

namespace CamelotEngine
{
	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(createSamplerStateImpl(), &CoreObject::_deleteDelayed);
		samplerState->setThisPtr(samplerState);
		samplerState->initialize(desc);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(createDepthStencilStateImpl(), &CoreObject::_deleteDelayed);
		depthStencilState->setThisPtr(depthStencilState);
		depthStencilState->initialize(desc);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(createRasterizerStateImpl(), &CoreObject::_deleteDelayed);
		rasterizerState->setThisPtr(rasterizerState);
		rasterizerState->initialize(desc);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = BlendStatePtr(createBlendStateImpl(), &CoreObject::_deleteDelayed);
		blendState->setThisPtr(blendState);
		blendState->initialize(desc);

		return blendState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(createSamplerStateImpl(), &CoreObject::_deleteDelayed);
		samplerState->setThisPtr(samplerState);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(createDepthStencilStateImpl(), &CoreObject::_deleteDelayed);
		depthStencilState->setThisPtr(depthStencilState);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createEmptyRasterizerState() const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(createRasterizerStateImpl(), &CoreObject::_deleteDelayed);
		rasterizerState->setThisPtr(rasterizerState);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createEmptyBlendState() const
	{
		BlendStatePtr blendState = BlendStatePtr(createBlendStateImpl(), &CoreObject::_deleteDelayed);
		blendState->setThisPtr(blendState);

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

	SamplerState* RenderStateManager::createSamplerStateImpl() const
	{
		return new SamplerState();
	}

	BlendState* RenderStateManager::createBlendStateImpl() const
	{
		return new BlendState();
	}

	RasterizerState* RenderStateManager::createRasterizerStateImpl() const
	{
		return new RasterizerState();
	}

	DepthStencilState* RenderStateManager::createDepthStencilStateImpl() const
	{
		return new DepthStencilState();
	}
}
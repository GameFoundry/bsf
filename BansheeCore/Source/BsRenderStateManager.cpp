#include "BsRenderStateManager.h"
#include "BsSamplerState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"

namespace BansheeEngine
{
	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = bs_core_ptr<SamplerState, GenAlloc>(new (bs_alloc<SamplerState>()) SamplerState(desc));
		samplerState->_setThisPtr(samplerState);
		samplerState->initialize();

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = bs_core_ptr<DepthStencilState, GenAlloc>(new (bs_alloc<DepthStencilState>()) DepthStencilState(desc));
		depthStencilState->_setThisPtr(depthStencilState);
		depthStencilState->initialize();

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = bs_core_ptr<RasterizerState, GenAlloc>(new (bs_alloc<RasterizerState>()) RasterizerState(desc));
		rasterizerState->_setThisPtr(rasterizerState);
		rasterizerState->initialize();

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = bs_core_ptr<BlendState, GenAlloc>(new (bs_alloc<BlendState>()) BlendState(desc));
		blendState->_setThisPtr(blendState);
		blendState->initialize();

		return blendState;
	}

	SamplerStatePtr RenderStateManager::createEmptySamplerState() const
	{
		SamplerStatePtr samplerState = bs_core_ptr<SamplerState, GenAlloc>(
			new (bs_alloc<SamplerState>()) SamplerState(SAMPLER_STATE_DESC()));
		samplerState->_setThisPtr(samplerState);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::createEmptyDepthStencilState() const
	{
		DepthStencilStatePtr depthStencilState = bs_core_ptr<DepthStencilState, GenAlloc>(
			new (bs_alloc<DepthStencilState>()) DepthStencilState(DEPTH_STENCIL_STATE_DESC()));
		depthStencilState->_setThisPtr(depthStencilState);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::createEmptyRasterizerState() const
	{
		RasterizerStatePtr rasterizerState = bs_core_ptr<RasterizerState, GenAlloc>(
			new (bs_alloc<RasterizerState>()) RasterizerState(RASTERIZER_STATE_DESC()));
		rasterizerState->_setThisPtr(rasterizerState);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::createEmptyBlendState() const
	{
		BlendStatePtr blendState = bs_core_ptr<BlendState, GenAlloc>(
			new (bs_alloc<BlendState>()) BlendState(BLEND_STATE_DESC()));
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

	SPtr<SamplerStateCore> RenderStateCoreManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SPtr<SamplerStateCore> samplerState = createSamplerStateInternal(desc);
		samplerState->initialize();

		return samplerState;
	}

	SPtr<DepthStencilStateCore> RenderStateCoreManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilStateCore> depthStencilState = createDepthStencilStateInternal(desc);
		depthStencilState->initialize();

		return depthStencilState;
	}

	SPtr<RasterizerStateCore> RenderStateCoreManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerStateCore> rasterizerState = createRasterizerStateInternal(desc);
		rasterizerState->initialize();

		return rasterizerState;
	}

	SPtr<BlendStateCore> RenderStateCoreManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		SPtr<BlendStateCore> blendState = createBlendStateInternal(desc);
		blendState->initialize();

		return blendState;
	}

	SPtr<SamplerStateCore> RenderStateCoreManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
	{
		SPtr<SamplerStateCore> samplerState = bs_shared_ptr<SamplerStateCore, GenAlloc>(new (bs_alloc<SamplerStateCore>()) SamplerStateCore(desc));

		return samplerState;
	}

	SPtr<DepthStencilStateCore> RenderStateCoreManager::createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilStateCore> depthStencilState = bs_shared_ptr<DepthStencilStateCore, GenAlloc>(new (bs_alloc<DepthStencilStateCore>()) DepthStencilStateCore(desc));

		return depthStencilState;
	}

	SPtr<RasterizerStateCore> RenderStateCoreManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerStateCore> rasterizerState = bs_shared_ptr<RasterizerStateCore, GenAlloc>(new (bs_alloc<RasterizerStateCore>()) RasterizerStateCore(desc));

		return rasterizerState;
	}

	SPtr<BlendStateCore> RenderStateCoreManager::createBlendStateInternal(const BLEND_STATE_DESC& desc) const
	{
		SPtr<BlendStateCore> blendState = bs_shared_ptr<BlendStateCore, GenAlloc>(new (bs_alloc<BlendStateCore>()) BlendStateCore(desc));

		return blendState;
	}
}
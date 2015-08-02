#include "BsRenderStateManager.h"
#include "BsSamplerState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsBlendState.h"

namespace BansheeEngine
{
	template<bool Core>
	void TRenderStateManager<Core>::notifySamplerStateCreated(const SAMPLER_STATE_DESC& desc, const SPtr<SamplerStateType>& state) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedSamplerStates[desc] = state;
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyBlendStateCreated(const BLEND_STATE_DESC& desc, const SPtr<BlendStateType>& state) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedBlendStates[desc] = state;
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyRasterizerStateCreated(const RASTERIZER_STATE_DESC& desc, const SPtr<RasterizerStateType>& state) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedRasterizerStates[desc] = state;
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyDepthStencilStateCreated(const DEPTH_STENCIL_STATE_DESC& desc, const SPtr<DepthStencilStateType>& state) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedDepthStencilStates[desc] = state;
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifySamplerStateDestroyed(const SAMPLER_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedSamplerStates.erase(desc);
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyBlendStateDestroyed(const BLEND_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedBlendStates.erase(desc);
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyRasterizerStateDestroyed(const RASTERIZER_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedRasterizerStates.erase(desc);
	}

	template<bool Core>
	void TRenderStateManager<Core>::notifyDepthStencilStateDestroyed(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		mCachedDepthStencilStates.erase(desc);
	}

	template<bool Core>
	SPtr<typename TRenderStateManager<Core>::SamplerStateType> TRenderStateManager<Core>::findCachedState(const SAMPLER_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		auto iterFind = mCachedSamplerStates.find(desc);
		if (iterFind != mCachedSamplerStates.end())
			return iterFind->second.lock();

		return nullptr;
	}

	template<bool Core>
	SPtr<typename TRenderStateManager<Core>::BlendStateType> TRenderStateManager<Core>::findCachedState(const BLEND_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		auto iterFind = mCachedBlendStates.find(desc);
		if (iterFind != mCachedBlendStates.end())
			return iterFind->second.lock();

		return nullptr;
	}

	template<bool Core>
	SPtr<typename TRenderStateManager<Core>::RasterizerStateType> TRenderStateManager<Core>::findCachedState(const RASTERIZER_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		auto iterFind = mCachedRasterizerStates.find(desc);
		if (iterFind != mCachedRasterizerStates.end())
			return iterFind->second.lock();

		return nullptr;
	}

	template<bool Core>
	SPtr<typename TRenderStateManager<Core>::DepthStencilStateType> TRenderStateManager<Core>::findCachedState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		BS_LOCK_MUTEX(mMutex);

		auto iterFind = mCachedDepthStencilStates.find(desc);
		if (iterFind != mCachedDepthStencilStates.end())
			return iterFind->second.lock();

		return nullptr;
	}

	template class TRenderStateManager < true > ;
	template class TRenderStateManager < false >;

	SamplerStatePtr RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		SPtr<SamplerState> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_core_ptr<SamplerState, GenAlloc>(new (bs_alloc<SamplerState>()) SamplerState(desc));
			state->_setThisPtr(state);
			state->initialize();

			notifySamplerStateCreated(desc, state);
		}

		return state;
	}

	DepthStencilStatePtr RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilState> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_core_ptr<DepthStencilState, GenAlloc>(new (bs_alloc<DepthStencilState>()) DepthStencilState(desc));
			state->_setThisPtr(state);
			state->initialize();

			notifyDepthStencilStateCreated(desc, state);
		}

		return state;
	}

	RasterizerStatePtr RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerState> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_core_ptr<RasterizerState, GenAlloc>(new (bs_alloc<RasterizerState>()) RasterizerState(desc));
			state->_setThisPtr(state);
			state->initialize();

			notifyRasterizerStateCreated(desc, state);
		}

		return state;
	}

	BlendStatePtr RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		SPtr<BlendState> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_core_ptr<BlendState, GenAlloc>(new (bs_alloc<BlendState>()) BlendState(desc));
			state->_setThisPtr(state);
			state->initialize();

			notifyBlendStateCreated(desc, state);
		}

		return state;
	}

	SamplerStatePtr RenderStateManager::_createSamplerStatePtr(const SAMPLER_STATE_DESC& desc) const
	{
		SamplerStatePtr samplerState = bs_core_ptr<SamplerState, GenAlloc>(
			new (bs_alloc<SamplerState>()) SamplerState(desc));
		samplerState->_setThisPtr(samplerState);

		return samplerState;
	}

	DepthStencilStatePtr RenderStateManager::_createDepthStencilStatePtr(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		DepthStencilStatePtr depthStencilState = bs_core_ptr<DepthStencilState, GenAlloc>(
			new (bs_alloc<DepthStencilState>()) DepthStencilState(desc));
		depthStencilState->_setThisPtr(depthStencilState);

		return depthStencilState;
	}

	RasterizerStatePtr RenderStateManager::_createRasterizerStatePtr(const RASTERIZER_STATE_DESC& desc) const
	{
		RasterizerStatePtr rasterizerState = bs_core_ptr<RasterizerState, GenAlloc>(
			new (bs_alloc<RasterizerState>()) RasterizerState(desc));
		rasterizerState->_setThisPtr(rasterizerState);

		return rasterizerState;
	}

	BlendStatePtr RenderStateManager::_createBlendStatePtr(const BLEND_STATE_DESC& desc) const
	{
		BlendStatePtr blendState = bs_core_ptr<BlendState, GenAlloc>(
			new (bs_alloc<BlendState>()) BlendState(desc));
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

	const SPtr<SamplerStateCore>& RenderStateCoreManager::getDefaultSamplerState() const
	{
		if (mDefaultSamplerState == nullptr)
			mDefaultSamplerState = createSamplerState(SAMPLER_STATE_DESC());

		return mDefaultSamplerState;
	}

	const SPtr<BlendStateCore>& RenderStateCoreManager::getDefaultBlendState() const
	{
		if (mDefaultBlendState == nullptr)
			mDefaultBlendState = createBlendState(BLEND_STATE_DESC());

		return mDefaultBlendState;
	}

	const SPtr<RasterizerStateCore>& RenderStateCoreManager::getDefaultRasterizerState() const
	{
		if (mDefaultRasterizerState == nullptr)
			mDefaultRasterizerState = createRasterizerState(RASTERIZER_STATE_DESC());

		return mDefaultRasterizerState;
	}

	const SPtr<DepthStencilStateCore>& RenderStateCoreManager::getDefaultDepthStencilState() const
	{
		if (mDefaultDepthStencilState == nullptr)
			mDefaultDepthStencilState = createDepthStencilState(DEPTH_STENCIL_STATE_DESC());

		return mDefaultDepthStencilState;
	}

	SPtr<SamplerStateCore> RenderStateCoreManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const
	{
		SPtr<SamplerStateCore> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_shared_ptr<SamplerStateCore, GenAlloc>(new (bs_alloc<SamplerStateCore>()) SamplerStateCore(desc));
			state->_setThisPtr(state);

			notifySamplerStateCreated(desc, state);
		}

		return state;
	}

	SPtr<DepthStencilStateCore> RenderStateCoreManager::createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		SPtr<DepthStencilStateCore> state = findCachedState(desc);
		if (state == nullptr)
		{
			SPtr<DepthStencilStateCore> state = bs_shared_ptr<DepthStencilStateCore, GenAlloc>(new (bs_alloc<DepthStencilStateCore>()) DepthStencilStateCore(desc));
			state->_setThisPtr(state);

			notifyDepthStencilStateCreated(desc, state);
		}

		return state;
	}

	SPtr<RasterizerStateCore> RenderStateCoreManager::createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const
	{
		SPtr<RasterizerStateCore> state = findCachedState(desc);
		if (state == nullptr)
		{
			SPtr<RasterizerStateCore> state = bs_shared_ptr<RasterizerStateCore, GenAlloc>(new (bs_alloc<RasterizerStateCore>()) RasterizerStateCore(desc));
			state->_setThisPtr(state);

			notifyRasterizerStateCreated(desc, state);
		}

		return state;
	}

	SPtr<BlendStateCore> RenderStateCoreManager::createBlendStateInternal(const BLEND_STATE_DESC& desc) const
	{
		SPtr<BlendStateCore> state = findCachedState(desc);
		if (state == nullptr)
		{
			state = bs_shared_ptr<BlendStateCore, GenAlloc>(new (bs_alloc<BlendStateCore>()) BlendStateCore(desc));
			state->_setThisPtr(state);

			notifyBlendStateCreated(desc, state);
		}

		return state;
	}
}
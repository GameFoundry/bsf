#include "CmD3D11RenderStateManager.h"
#include "CmD3D11SamplerState.h"
#include "CmD3D11DepthStencilState.h"
#include "CmD3D11RasterizerState.h"
#include "CmD3D11BlendState.h"

namespace BansheeEngine
{
	SamplerStatePtr D3D11RenderStateManager::createSamplerStateImpl() const
	{
		SamplerStatePtr samplerState = bs_core_ptr<D3D11SamplerState, PoolAlloc>(new (bs_alloc<D3D11SamplerState, PoolAlloc>()) D3D11SamplerState());
		return samplerState;
	}

	BlendStatePtr D3D11RenderStateManager::createBlendStateImpl() const
	{
		BlendStatePtr blendState = bs_core_ptr<D3D11BlendState, PoolAlloc>(new (bs_alloc<D3D11BlendState, PoolAlloc>()) D3D11BlendState());
		return blendState;
	}

	RasterizerStatePtr D3D11RenderStateManager::createRasterizerStateImpl() const
	{
		RasterizerStatePtr rasterizerState = bs_core_ptr<D3D11RasterizerState, PoolAlloc>(new (bs_alloc<D3D11RasterizerState, PoolAlloc>()) D3D11RasterizerState());
		return rasterizerState;
	}

	DepthStencilStatePtr D3D11RenderStateManager::createDepthStencilStateImpl() const
	{
		DepthStencilStatePtr depthStencilState = bs_core_ptr<D3D11DepthStencilState, PoolAlloc>(new (bs_alloc<D3D11DepthStencilState, PoolAlloc>()) D3D11DepthStencilState());
		return depthStencilState;
	}
}
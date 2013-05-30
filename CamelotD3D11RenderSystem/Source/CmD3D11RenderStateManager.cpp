#include "CmD3D11RenderStateManager.h"
#include "CmD3D11SamplerState.h"
#include "CmD3D11DepthStencilState.h"
#include "CmD3D11RasterizerState.h"
#include "CmD3D11BlendState.h"

namespace CamelotFramework
{
	SamplerStatePtr D3D11RenderStateManager::createSamplerStateImpl() const
	{
		SamplerStatePtr samplerState = cm_core_ptr<D3D11SamplerState, PoolAlloc>(new (cm_alloc<D3D11SamplerState, PoolAlloc>()) D3D11SamplerState());
		return samplerState;
	}

	BlendStatePtr D3D11RenderStateManager::createBlendStateImpl() const
	{
		BlendStatePtr blendState = cm_core_ptr<D3D11BlendState, PoolAlloc>(new (cm_alloc<D3D11BlendState, PoolAlloc>()) D3D11BlendState());
		return blendState;
	}

	RasterizerStatePtr D3D11RenderStateManager::createRasterizerStateImpl() const
	{
		RasterizerStatePtr rasterizerState = cm_core_ptr<D3D11RasterizerState, PoolAlloc>(new (cm_alloc<D3D11RasterizerState, PoolAlloc>()) D3D11RasterizerState());
		return rasterizerState;
	}

	DepthStencilStatePtr D3D11RenderStateManager::createDepthStencilStateImpl() const
	{
		DepthStencilStatePtr depthStencilState = cm_core_ptr<D3D11DepthStencilState, PoolAlloc>(new (cm_alloc<D3D11DepthStencilState, PoolAlloc>()) D3D11DepthStencilState());
		return depthStencilState;
	}
}
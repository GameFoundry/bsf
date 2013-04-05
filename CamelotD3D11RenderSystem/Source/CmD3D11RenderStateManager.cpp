#include "CmD3D11RenderStateManager.h"
#include "CmD3D11SamplerState.h"
#include "CmD3D11DepthStencilState.h"
#include "CmD3D11RasterizerState.h"
#include "CmD3D11BlendState.h"

namespace CamelotEngine
{
	SamplerStatePtr D3D11RenderStateManager::createSamplerStateImpl() const
	{
		SamplerStatePtr samplerState = SamplerStatePtr(CM_NEW(D3D11SamplerState, PoolAlloc) D3D11SamplerState(), &CoreObject::_deleteDelayed<D3D11SamplerState, PoolAlloc>);
		return samplerState;
	}

	BlendStatePtr D3D11RenderStateManager::createBlendStateImpl() const
	{
		BlendStatePtr blendState = BlendStatePtr(CM_NEW(D3D11BlendState, PoolAlloc) D3D11BlendState(), &CoreObject::_deleteDelayed<D3D11BlendState, PoolAlloc>);
		return blendState;
	}

	RasterizerStatePtr D3D11RenderStateManager::createRasterizerStateImpl() const
	{
		RasterizerStatePtr rasterizerState = RasterizerStatePtr(CM_NEW(D3D11RasterizerState, PoolAlloc) D3D11RasterizerState(), &CoreObject::_deleteDelayed<D3D11RasterizerState, PoolAlloc>);
		return rasterizerState;
	}

	DepthStencilStatePtr D3D11RenderStateManager::createDepthStencilStateImpl() const
	{
		DepthStencilStatePtr depthStencilState = DepthStencilStatePtr(CM_NEW(D3D11DepthStencilState, PoolAlloc) D3D11DepthStencilState(), &CoreObject::_deleteDelayed<D3D11DepthStencilState, PoolAlloc>);
		return depthStencilState;
	}
}
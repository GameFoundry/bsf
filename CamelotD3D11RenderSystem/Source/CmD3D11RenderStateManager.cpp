#include "CmD3D11RenderStateManager.h"
#include "CmD3D11SamplerState.h"
#include "CmD3D11DepthStencilState.h"
#include "CmD3D11RasterizerState.h"
#include "CmD3D11BlendState.h"

namespace CamelotEngine
{
	SamplerStatePtr D3D11RenderStateManager::createSamplerState(const SAMPLER_STATE_DESC& desc) const
	{
		D3D11SamplerStatePtr samplerState = D3D11SamplerStatePtr(new D3D11SamplerState());
		samplerState->initialize(desc);

		return samplerState;
	}

	DepthStencilStatePtr D3D11RenderStateManager::createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const
	{
		D3D11DepthStencilStatePtr depthStencilState = D3D11DepthStencilStatePtr(new D3D11DepthStencilState());
		depthStencilState->initialize(desc);

		return depthStencilState;
	}

	RasterizerStatePtr D3D11RenderStateManager::createRasterizerState(const RASTERIZER_STATE_DESC& desc) const
	{
		D3D11RasterizerStatePtr rasterizerState = D3D11RasterizerStatePtr(new D3D11RasterizerState());
		rasterizerState->initialize(desc);

		return rasterizerState;
	}

	BlendStatePtr D3D11RenderStateManager::createBlendState(const BLEND_STATE_DESC& desc) const
	{
		D3D11BlendStatePtr blendState = D3D11BlendStatePtr(new D3D11BlendState());
		blendState->initialize(desc);

		return blendState;
	}

	SamplerStatePtr D3D11RenderStateManager::createEmptySamplerState() const
	{
		return D3D11SamplerStatePtr(new D3D11SamplerState());
	}

	DepthStencilStatePtr D3D11RenderStateManager::createEmptyDepthStencilState() const
	{
		return D3D11DepthStencilStatePtr(new D3D11DepthStencilState());
	}

	RasterizerStatePtr D3D11RenderStateManager::createEmptyRasterizerState() const
	{
		return D3D11RasterizerStatePtr(new D3D11RasterizerState());
	}

	BlendStatePtr D3D11RenderStateManager::createEmptyBlendState() const
	{
		return D3D11BlendStatePtr(new D3D11BlendState());
	}
}
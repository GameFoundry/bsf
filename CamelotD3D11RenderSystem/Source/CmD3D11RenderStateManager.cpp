#include "CmD3D11RenderStateManager.h"
#include "CmD3D11SamplerState.h"
#include "CmD3D11DepthStencilState.h"
#include "CmD3D11RasterizerState.h"
#include "CmD3D11BlendState.h"

namespace CamelotEngine
{
	SamplerState* D3D11RenderStateManager::createSamplerStateImpl() const
	{
		return new D3D11SamplerState();
	}

	BlendState* D3D11RenderStateManager::createBlendStateImpl() const
	{
		return new D3D11BlendState();
	}

	RasterizerState* D3D11RenderStateManager::createRasterizerStateImpl() const
	{
		return new D3D11RasterizerState();
	}

	DepthStencilState* D3D11RenderStateManager::createDepthStencilStateImpl() const
	{
		return new D3D11DepthStencilState();
	}
}
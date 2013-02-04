#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderStateManager.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderStateManager : public RenderStateManager
	{
	protected:
		virtual SamplerState* createSamplerStateImpl() const;
		virtual BlendState* createBlendStateImpl() const;
		virtual RasterizerState* createRasterizerStateImpl() const;
		virtual DepthStencilState* createDepthStencilStateImpl() const;
	};
}
#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmRenderStateManager.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11RenderStateManager : public RenderStateManager
	{
	public:
		/**
		 * @copydoc RenderStateManager::createSamplerState()
		 */
		SamplerStatePtr createSamplerState(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @copydoc RenderStateManager::createDepthStencilState()
		 */
		DepthStencilStatePtr createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**
		 * @copydoc RenderStateManager::createRasterizerState()
		 */
		RasterizerStatePtr createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @copydoc RenderStateManager::createBlendState()
		 */
		BlendStatePtr createBlendState(const BLEND_STATE_DESC& desc) const;

		/**
		 * @copydoc RenderStateManager::createEmptySamplerState()
		 */
		SamplerStatePtr createEmptySamplerState() const;

		/**
		 * @copydoc RenderStateManager::createEmptyDepthStencilState()
		 */
		DepthStencilStatePtr createEmptyDepthStencilState() const;

		/**
		 * @copydoc RenderStateManager::createEmptyRasterizerState()
		 */
		RasterizerStatePtr createEmptyRasterizerState() const;

		/**
		 * @copydoc RenderStateManager::createEmptyBlendState()
		 */
		BlendStatePtr createEmptyBlendState() const;
	};
}
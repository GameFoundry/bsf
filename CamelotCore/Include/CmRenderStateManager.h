#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	class CM_EXPORT RenderStateManager : public Module<RenderStateManager>
	{
	public:
		/**
		 * @brief	Creates and initializes a new SamplerState.
		 */
		SamplerStatePtr createSamplerState(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @brief	Creates and initializes a new DepthStencilState.
		 */
		DepthStencilStatePtr createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**
		 * @brief	Creates and initializes a new RasterizerState.
		 */
		RasterizerStatePtr createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @brief	Creates and initializes a new BlendState.
		 */
		BlendStatePtr createBlendState(const BLEND_STATE_DESC& desc) const;

		/**
		 * @brief	Creates a completely empty and uninitialized SamplerState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		SamplerStatePtr createEmptySamplerState() const;

		/**
		 * @brief	Creates a completely empty and uninitialized DepthStencilState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		DepthStencilStatePtr createEmptyDepthStencilState() const;

		/**
		 * @brief	Creates a completely empty and uninitialized RasterizerState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		RasterizerStatePtr createEmptyRasterizerState() const;

		/**
		 * @brief	Creates a completely empty and uninitialized BlendState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		BlendStatePtr createEmptyBlendState() const;

		const SamplerStatePtr& getDefaultSamplerState() const;
		const BlendStatePtr& getDefaultBlendState() const;
		const RasterizerStatePtr& getDefaultRasterizerState() const;
		const DepthStencilStatePtr& getDefaultDepthStencilState() const;

	protected:
		virtual SamplerStatePtr createSamplerStateImpl() const;
		virtual BlendStatePtr createBlendStateImpl() const;
		virtual RasterizerStatePtr createRasterizerStateImpl() const;
		virtual DepthStencilStatePtr createDepthStencilStateImpl() const;

	private:
		mutable SamplerStatePtr mDefaultSamplerState;
		mutable BlendStatePtr mDefaultBlendState;
		mutable RasterizerStatePtr mDefaultRasterizerState;
		mutable DepthStencilStatePtr mDefaultDepthStencilState;
	};
}
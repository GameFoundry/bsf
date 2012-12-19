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
		virtual SamplerStatePtr createSamplerState(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @brief	Creates and initializes a new DepthStencilState.
		 */
		virtual DepthStencilStatePtr createDepthStencilState(const DEPTH_STENCIL_DESC& desc) const;

		/**
		 * @brief	Creates a completely empty and uninitialized SamplerState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		virtual SamplerStatePtr createEmptySamplerState() const;

		/**
		 * @brief	Creates a completely empty and uninitialized DepthStencilState.
		 * 			Should only be used for VERY specific purposes, like deserialization,
		 * 			as it requires additional manual initialization that is not required normally.
		 */
		virtual DepthStencilStatePtr createEmptyDepthStencilState() const;
	};
}
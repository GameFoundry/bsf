//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of various render states.
	 */
	class BS_CORE_EXPORT RenderStateManager : public Module<RenderStateManager>
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

		/**
		 * @brief	Gets a sampler state initialized with default options.
		 */
		const SamplerStatePtr& getDefaultSamplerState() const;
		
		/**
		 * @brief	Gets a blend state initialized with default options.
		 */
		const BlendStatePtr& getDefaultBlendState() const;

		/**
		 * @brief	Gets a rasterizer state initialized with default options.
		 */
		const RasterizerStatePtr& getDefaultRasterizerState() const;

		/**
		 * @brief	Gets a depth stencil state initialized with default options.
		 */
		const DepthStencilStatePtr& getDefaultDepthStencilState() const;

	protected:
		/**
		 * @copydoc	createSamplerState
		 */
		virtual SamplerStatePtr createSamplerStateImpl() const;

		/**
		 * @copydoc	createBlendState
		 */
		virtual BlendStatePtr createBlendStateImpl() const;

		/**
		 * @copydoc	createRasterizerState
		 */
		virtual RasterizerStatePtr createRasterizerStateImpl() const;

		/**
		 * @copydoc	createDepthStencilState
		 */
		virtual DepthStencilStatePtr createDepthStencilStateImpl() const;

	private:
		mutable SamplerStatePtr mDefaultSamplerState;
		mutable BlendStatePtr mDefaultBlendState;
		mutable RasterizerStatePtr mDefaultRasterizerState;
		mutable DepthStencilStatePtr mDefaultDepthStencilState;
	};
}
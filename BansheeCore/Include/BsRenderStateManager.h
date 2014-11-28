#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of various render states.
	 */
	class BS_CORE_EXPORT RenderStateManager : public Module <RenderStateManager>
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

	private:
		mutable SamplerStatePtr mDefaultSamplerState;
		mutable BlendStatePtr mDefaultBlendState;
		mutable RasterizerStatePtr mDefaultRasterizerState;
		mutable DepthStencilStatePtr mDefaultDepthStencilState;
	};

	/**
	 * @brief	Handles creation of various render states.
	 */
	class BS_CORE_EXPORT RenderStateCoreManager : public Module<RenderStateCoreManager>
	{
	public:
		/**
		 * @copydoc	RenderStateManager::createSamplerState
		 */
		SPtr<SamplerStateCore> createSamplerState(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @copydoc	RenderStateManager::createDepthStencilState
		 */
		SPtr<DepthStencilStateCore> createDepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**
		 * @copydoc	RenderStateManager::createRasterizerState
		 */
		SPtr<RasterizerStateCore> createRasterizerState(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @copydoc	RenderStateManager::createBlendState
		 */
		SPtr<BlendStateCore> createBlendState(const BLEND_STATE_DESC& desc) const;

		/**
		 * @brief	Gets a sampler state initialized with default options.
		 */
		const SPtr<SamplerStateCore>& getDefaultSamplerState() const;

		/**
		 * @brief	Gets a blend state initialized with default options.
		 */
		const SPtr<BlendStateCore>& getDefaultBlendState() const;

		/**
		 * @brief	Gets a rasterizer state initialized with default options.
		 */
		const SPtr<RasterizerStateCore>& getDefaultRasterizerState() const;

		/**
		 * @brief	Gets a depth stencil state initialized with default options.
		 */
		const SPtr<DepthStencilStateCore>& getDefaultDepthStencilState() const;

	private:
		mutable SPtr<SamplerStateCore> mDefaultSamplerState;
		mutable SPtr<BlendStateCore> mDefaultBlendState;
		mutable SPtr<RasterizerStateCore> mDefaultRasterizerState;
		mutable SPtr<DepthStencilStateCore> mDefaultDepthStencilState;

	protected:
		friend class SamplerState;
		friend class BlendState;
		friend class RasterizerState;
		friend class DepthStencilState;

		/**
		 * @copydoc	createSamplerState
		 */
		virtual SPtr<SamplerStateCore> createSamplerStateInternal(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @copydoc	createBlendState
		 */
		virtual SPtr<BlendStateCore> createBlendStateInternal(const BLEND_STATE_DESC& desc) const;

		/**
		 * @copydoc	createRasterizerState
		 */
		virtual SPtr<RasterizerStateCore> createRasterizerStateInternal(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @copydoc	createDepthStencilState
		 */
		virtual SPtr<DepthStencilStateCore> createDepthStencilStateInternal(const DEPTH_STENCIL_STATE_DESC& desc) const;
	};
}
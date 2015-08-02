#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"
#include "BsBlendState.h"
#include "BsRasterizerState.h"
#include "BsDepthStencilState.h"
#include "BsSamplerState.h"

namespace BansheeEngine
{
	/**
	 * @brief	Common methods and data for both sim and core thread versions
	 *			of RenderStateManager.
	 */
	template<bool Core>
	class BS_CORE_EXPORT TRenderStateManager
	{
	public:
		virtual ~TRenderStateManager() { }

	protected:
		template<bool Core> struct TSamplerStateType { };
		template<> struct TSamplerStateType < false > { typedef SamplerState Type; };
		template<> struct TSamplerStateType < true > { typedef SamplerStateCore Type; };

		template<bool Core> struct TBlendStateType { };
		template<> struct TBlendStateType < false > { typedef BlendState Type; };
		template<> struct TBlendStateType < true > { typedef BlendStateCore Type; };

		template<bool Core> struct TRasterizerStateType { };
		template<> struct TRasterizerStateType < false > { typedef RasterizerState Type; };
		template<> struct TRasterizerStateType < true > { typedef RasterizerStateCore Type; };

		template<bool Core> struct TDepthStencilStateType { };
		template<> struct TDepthStencilStateType < false > { typedef DepthStencilState Type; };
		template<> struct TDepthStencilStateType < true > { typedef DepthStencilStateCore Type; };

		typedef typename TSamplerStateType<Core>::Type SamplerStateType;
		typedef typename TBlendStateType<Core>::Type BlendStateType;
		typedef typename TRasterizerStateType<Core>::Type RasterizerStateType;
		typedef typename TDepthStencilStateType<Core>::Type DepthStencilStateType;

		/**
		 * @brief	Triggered when a new sampler state is created. 
		 */
		void notifySamplerStateCreated(const SAMPLER_STATE_DESC& desc, const SPtr<SamplerStateType>& state) const;

		/**
		 * @brief	Triggered when a new sampler state is created. 
		 */
		void notifyBlendStateCreated(const BLEND_STATE_DESC& desc, const SPtr<BlendStateType>& state) const;

		/**
		 * @brief	Triggered when a new sampler state is created. 
		 */
		void notifyRasterizerStateCreated(const RASTERIZER_STATE_DESC& desc, const SPtr<RasterizerStateType>& state) const;

		/**
		 * @brief	Triggered when a new sampler state is created. 
		 */
		void notifyDepthStencilStateCreated(const DEPTH_STENCIL_STATE_DESC& desc, const SPtr<DepthStencilStateType>& state) const;

		/**
		 * @brief	Triggered when the last reference to a specific sampler state is destroyed, which
		 *			means we must clear our cached version as well.
		 */
		void notifySamplerStateDestroyed(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @brief	Triggered when the last reference to a specific blend state is destroyed, which
		 *			means we must clear our cached version as well.
		 */
		void notifyBlendStateDestroyed(const BLEND_STATE_DESC& desc) const;

		/**
		 * @brief	Triggered when the last reference to a specific rasterizer state is destroyed, which
		 *			means we must clear our cached version as well.
		 */
		void notifyRasterizerStateDestroyed(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @brief	Triggered when the last reference to a specific depth stencil state is destroyed, which
		 *			means we must clear our cached version as well.
		 */
		void notifyDepthStencilStateDestroyed(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**
		 * @brief	Attempts to find a cached sampler state corresponding to the provided descriptor. 
		 *			Returns null if one doesn't exist.
		 */
		SPtr<SamplerStateType> findCachedState(const SAMPLER_STATE_DESC& desc) const;

				/**
		 * @brief	Attempts to find a cached blend state corresponding to the provided descriptor. 
		 *			Returns null if one doesn't exist.
		 */
		SPtr<BlendStateType> findCachedState(const BLEND_STATE_DESC& desc) const;

				/**
		 * @brief	Attempts to find a cached rasterizer state corresponding to the provided descriptor. 
		 *			Returns null if one doesn't exist.
		 */
		SPtr<RasterizerStateType> findCachedState(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @brief	Attempts to find a cached depth-stencil state corresponding to the provided descriptor. 
		 *			Returns null if one doesn't exist.
		 */
		SPtr<DepthStencilStateType> findCachedState(const DEPTH_STENCIL_STATE_DESC& desc) const;

		mutable UnorderedMap<SAMPLER_STATE_DESC, std::weak_ptr<SamplerStateType>> mCachedSamplerStates;
		mutable UnorderedMap<BLEND_STATE_DESC, std::weak_ptr<BlendStateType>> mCachedBlendStates;
		mutable UnorderedMap<RASTERIZER_STATE_DESC, std::weak_ptr<RasterizerStateType>> mCachedRasterizerStates;
		mutable UnorderedMap<DEPTH_STENCIL_STATE_DESC, std::weak_ptr<DepthStencilStateType>> mCachedDepthStencilStates;

		BS_MUTEX(mMutex);
	};

	/**
	 * @brief	Handles creation of various render states.
	 */
	class BS_CORE_EXPORT RenderStateManager : public Module <RenderStateManager>, TRenderStateManager<false>
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
		 * @brief	Creates an uninitialized sampler state. Requires manual initialization
		 *			after creation.
		 *
		 * @note	Internal method.
		 */
		SamplerStatePtr _createSamplerStatePtr(const SAMPLER_STATE_DESC& desc) const;

		/**
		 * @brief	Creates an uninitialized depth-stencil state. Requires manual initialization
		 *			after creation.
		 *
		 * @note	Internal method.
		 */
		DepthStencilStatePtr _createDepthStencilStatePtr(const DEPTH_STENCIL_STATE_DESC& desc) const;

		/**
		 * @brief	Creates an uninitialized rasterizer state. Requires manual initialization
		 *			after creation.
		 *
		 * @note	Internal method.
		 */
		RasterizerStatePtr _createRasterizerStatePtr(const RASTERIZER_STATE_DESC& desc) const;

		/**
		 * @brief	Creates an uninitialized blend state. Requires manual initialization
		 *			after creation.
		 *
		 * @note	Internal method.
		 */
		BlendStatePtr _createBlendStatePtr(const BLEND_STATE_DESC& desc) const;

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
		friend class SamplerState;
		friend class BlendState;
		friend class RasterizerState;
		friend class DepthStencilState;

		mutable SamplerStatePtr mDefaultSamplerState;
		mutable BlendStatePtr mDefaultBlendState;
		mutable RasterizerStatePtr mDefaultRasterizerState;
		mutable DepthStencilStatePtr mDefaultDepthStencilState;
	};

	/**
	 * @brief	Handles creation of various render states.
	 */
	class BS_CORE_EXPORT RenderStateCoreManager : public Module<RenderStateCoreManager>, TRenderStateManager<true>
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
		friend class SamplerStateCore;
		friend class BlendStateCore;
		friend class RasterizerStateCore;
		friend class DepthStencilStateCore;

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
#pragma once

#include "BsCorePrerequisites.h"
#include "BsMatrix4.h"
#include "BsPixelUtil.h"
#include "BsTexture.h"
#include "BsColor.h"
#include "BsIReflectable.h"
#include "BsCoreObject.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Structure used for initializing a SamplerState.
	 *
	 * @see		SamplerState
	 */
	struct BS_CORE_EXPORT SAMPLER_STATE_DESC
	{
		SAMPLER_STATE_DESC()
			: minFilter(FO_LINEAR), magFilter(FO_LINEAR), mipFilter(FO_POINT), 
			maxAniso(0), mipmapBias(0), comparisonFunc(CMPF_ALWAYS_FAIL), mipMin(-FLT_MAX), 
			mipMax(FLT_MAX), borderColor(Color::White)
		{ }

		bool operator==(const SAMPLER_STATE_DESC& rhs) const;

		UVWAddressingMode addressMode;
		FilterOptions minFilter;
		FilterOptions magFilter;
		FilterOptions mipFilter;
		UINT32 maxAniso;
		float mipmapBias;
		float mipMin;
		float mipMax;
		Color borderColor;
		CompareFunction comparisonFunc;
	};

	/**
	 * @brief	Properties of SamplerState. Shared between sim and core thread versions of SamplerState.
	 */
	class BS_CORE_EXPORT SamplerProperties
	{
	public:
		SamplerProperties(const SAMPLER_STATE_DESC& desc);

		/**
		 * @brief	Returns texture addressing mode for each possible texture coordinate. Addressing
		 *			modes determine how are texture coordinates outside of [0, 1] range handled.
		 */
		const UVWAddressingMode& getTextureAddressingMode() const { return mData.addressMode; }

		/**
		 * @brief	Gets the filtering used when sampling from a texture.
		 */
        FilterOptions getTextureFiltering(FilterType ftpye) const;

		/**
		 * @brief	Gets the anisotropy level. Higher anisotropy means better filtering
		 *			for textures displayed on an angled slope relative to the viewer.
		 */
		unsigned int getTextureAnisotropy() const { return mData.maxAniso; }

		/**
		 * @brief	Gets a function that compares sampled data with existing sampled data.
		 */
		CompareFunction getComparisonFunction() const { return mData.comparisonFunc; }

		/**
		* @brief	Mipmap bias allows you to adjust the mipmap selection calculation. Negative values 
		*			force a larger mipmap to be used, and positive values smaller. Units are in values
		*			of mip levels, so -1 means use a mipmap one level higher than default.
		*/
		float getTextureMipmapBias() const { return mData.mipmapBias; }

		/**
		 * @brief	Returns the minimum mip map level.
		 */
		float getMinimumMip() const { return mData.mipMin; }

		/**
		 * @brief	Returns the maximum mip map level.
		 */
		float getMaximumMip() const { return mData.mipMax; }

		/**
		 * @brief	Gets the border color that will be used when border texture addressing is used
		 *			and texture address is outside of the valid range.
		 */
		const Color& getBorderColor() const;

		/**
		 * @brief	Returns the hash value generated from the sampler state properties.
		 */
		UINT64 getHash() const { return mHash; }

		/**
		 * @brief	Returns the descriptor originally used for creating the sampler state.
		 */
		SAMPLER_STATE_DESC getDesc() const { return mData; }

	protected:
		friend class SamplerState;
		friend class SamplerStateCore;
		friend class SamplerStateRTTI;

		SAMPLER_STATE_DESC mData;
		UINT64 mHash;
	};

	/**
	 * @brief	Core thread version of SamplerState.
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT SamplerStateCore : public CoreObjectCore
	{
	public:
		virtual ~SamplerStateCore();

		/**
		 * @brief	Returns information about the sampler state.
		 */
		const SamplerProperties& getProperties() const;

		/**
		 * @brief	Returns the default sampler state.
		 */
		static const SPtr<SamplerStateCore>& getDefault();

	protected:
		friend class RenderStateCoreManager;

		SamplerStateCore(const SAMPLER_STATE_DESC& desc);

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		void initialize() override;

		/**
		 * @brief	Creates any API-specific state objects.
		 */
		virtual void createInternal() { }

		SamplerProperties mProperties;
	};

	/**
	 * @brief	Class representing the state of a texture sampler.
	 *	
	 * @note	Sampler units are used for retrieving and filtering data from
	 *			textures set in a GPU program. Sampler states are immutable.
	 *
	 *			Sim thread.
	 */
	class BS_CORE_EXPORT SamplerState : public IReflectable, public CoreObject
    {
    public:
		virtual ~SamplerState();

		/**
		 * @brief	Returns information about the sampler state.
		 */
		const SamplerProperties& getProperties() const;

		/**
		 * @brief	Retrieves a core implementation of the sampler state usable only from the
		 *			core thread.
		 */
		SPtr<SamplerStateCore> getCore() const;

		/**
		 * @brief	Creates a new sampler state using the provided descriptor structure.
		 */
		static SamplerStatePtr create(const SAMPLER_STATE_DESC& desc);

		/**
		 * @brief	Returns the default sampler state.
		 */
		static const SamplerStatePtr& getDefault();

		/**
		 * @brief	Generates a hash value from a sampler state descriptor.
		 */
		static UINT64 generateHash(const SAMPLER_STATE_DESC& desc);

	protected:
		SamplerState(const SAMPLER_STATE_DESC& desc);

		/**
		 * @copydoc	CoreObjectCore::createCore
		 */
		SPtr<CoreObjectCore> createCore() const override;

		SamplerProperties mProperties;

		friend class RenderStateManager;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class SamplerStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;
    };
}

/**
 * @brief	Hash value generator for SAMPLER_STATE_DESC.
 */
template<>
struct std::hash<BansheeEngine::SAMPLER_STATE_DESC>
{
	size_t operator()(const BansheeEngine::SAMPLER_STATE_DESC& value) const
	{
		return (size_t)BansheeEngine::SamplerState::generateHash(value);
	}
};
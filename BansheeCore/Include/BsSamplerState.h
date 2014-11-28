#pragma once

#include "BsCorePrerequisites.h"
#include "BsMatrix4.h"
#include "BsPixelUtil.h"
#include "BsTexture.h"
#include "BsColor.h"
#include "BsResource.h"

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
	 * @brief	Information about a sampler state.
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

	protected:
		friend class SamplerState;
		friend class SamplerStateRTTI;

		SAMPLER_STATE_DESC mData;
	};

	/**
	 * @brief	Core thread version of a sampler state.
	 *
	 * @see		SamplerState
	 *
	 * @note	Core thread.
	 */
	class BS_CORE_EXPORT SamplerStateCore : public CoreObjectCore
	{
	public:
		virtual ~SamplerStateCore() {}

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
	class BS_CORE_EXPORT SamplerState : public Resource
    {
    public:
		virtual ~SamplerState() {}

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
		static HSamplerState create(const SAMPLER_STATE_DESC& desc);

		/**
		 * @brief	Returns the default sampler state.
		 */
		static const SamplerStatePtr& getDefault();

	protected:
		SamplerState(const SAMPLER_STATE_DESC& desc);

		/**
		 * @copydoc	CoreObjectCore::createCore
		 */
		SPtr<CoreObjectCore> createCore() const;

		SamplerProperties mProperties;

		friend class RenderStateManager;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class SamplerStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}
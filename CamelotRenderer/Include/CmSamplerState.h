#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmMatrix4.h"
#include "CmString.h"
#include "CmPixelUtil.h"
#include "CmTexture.h"
#include "CmColor.h"
#include "CmResource.h"

namespace CamelotEngine 
{
	struct CM_EXPORT SAMPLER_STATE_DESC
	{
		SAMPLER_STATE_DESC()
			: minFilter(FO_LINEAR)
			, magFilter(FO_LINEAR)
			, mipFilter(FO_POINT)
			, maxAniso(0)
			, mipmapBias(0)
			, comparisonFunc(CMPF_ALWAYS_FAIL)
			, mipMin(-FLT_MAX)
			, mipMax(FLT_MAX)
			, borderColor(Color::White)
		{
		}

		UVWAddressingMode addressMode;
		FilterOptions minFilter;
		FilterOptions magFilter;
		FilterOptions mipFilter;
		unsigned int maxAniso;
		float mipmapBias;
		float mipMin;
		float mipMax;
		Color borderColor;
		CompareFunction comparisonFunc;
	};

	// TODO Low priority - Write doc explaining various states
	/**
	* @brief	Class representing the state of a single sampler unit during a Pass of a Technique, of a Material.
	*	
	* @note		Sampler units are pipelines for retrieving texture data for rendering onto
	*			your objects in the world.
	*
	*			Try not to make modifications to a created sampler state. Any modification will require the sampler state to
	*			be recreated internally (depending on used render system). It is better to have multiple SamplerState objects with different
	*			configurations and re-use them.
	*/
	class CM_EXPORT SamplerState : public Resource
    {
    public:
		virtual ~SamplerState() {}

        /** Gets the texture addressing mode for a given coordinate, 
		 	i.e. what happens at uv values above 1.0.
        @note
        	The default is TAM_WRAP i.e. the texture repeats over values of 1.0.
        */
		const UVWAddressingMode& getTextureAddressingMode() const { return mData.addressMode; }

        // get the texture filtering for the given type
        FilterOptions getTextureFiltering(FilterType ftpye) const;

        // get this layer texture anisotropy level
		unsigned int getTextureAnisotropy() const { return mData.maxAniso; }

		/**
		 * @brief	Gets a function that compares sampled data with existing sampled data.
		 */
		CompareFunction getComparisonFunction() const { return mData.comparisonFunc; }

		/** Gets the bias value applied to the mipmap calculation.
		@see TextureUnitState::setTextureMipmapBias
		*/
		float getTextureMipmapBias() const { return mData.mipmapBias; }

		/**
		 * @brief	Returns the minimum mip level limit.
		 */
		float getMinimumMip() const { return mData.mipMin; }

		/**
		 * @brief	Returns the maximum mip level limit.
		 */
		float getMaximumMip() const { return mData.mipMax; }

		/**
		 * @brief	Gets the border color
		 */
		const Color& getBorderColor() const;

		static SamplerStatePtr create(const SAMPLER_STATE_DESC& desc);

		/**
		 * @brief	Returns the default sampler state;
		 */
		static const SamplerStatePtr& getDefault();

	protected:
		friend class RenderStateManager;

		virtual void initialize(const SAMPLER_STATE_DESC& desc);

        SAMPLER_STATE_DESC mData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class SamplerStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
    };
}
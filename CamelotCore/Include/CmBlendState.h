#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmCommonEnums.h"

namespace BansheeEngine
{
	/**
	 * @brief	Structure that describes blend states for a single render
	 * 			target. Used internally by BLEND_STATE_DESC for initializing a BlendState.
	 * 			
	 * @see		BLEND_STATE_DESC
	 * @see		BlendState
	 */
	struct CM_EXPORT RENDER_TARGET_BLEND_STATE_DESC
	{
		RENDER_TARGET_BLEND_STATE_DESC()
			: blendEnable(false)
			, srcBlend(BF_ONE)
			, dstBlend(BF_ZERO)
			, blendOp(BO_ADD)
			, srcBlendAlpha(BF_ONE)
			, dstBlendAlpha(BF_ZERO)
			, blendOpAlpha(BO_ADD)
			, renderTargetWriteMask(0xFF)
		{ }

		bool blendEnable;
		BlendFactor srcBlend;
		BlendFactor dstBlend;
		BlendOperation blendOp;
		BlendFactor srcBlendAlpha;
		BlendFactor dstBlendAlpha;
		BlendOperation blendOpAlpha;
		// Enable write to RGBA channels separately by setting first four bits (0 - R, 1 - G, 2 - B, 3 - A)
		UINT8 renderTargetWriteMask;
	};

	/**
	 * @brief	Structure that describes render pipeline blend states. Used for initializing
	 * 			BlendState.
	 * 			
	 * @see		BlendState.
	 */
	struct CM_EXPORT BLEND_STATE_DESC
	{
		BLEND_STATE_DESC()
			: alphaToCoverageEnable(false)
			, independantBlendEnable(false)
		{ }

		bool alphaToCoverageEnable;
		bool independantBlendEnable;
		RENDER_TARGET_BLEND_STATE_DESC renderTargetDesc[CM_MAX_MULTIPLE_RENDER_TARGETS];
	};

	/**
	 * @brief	Render system pipeline state that allows you to modify how an object is rendered. 
	 * 			More exactly this state allows to you to control how is a rendered
	 * 			object blended with any previously renderer objects.
	 * 			
	 * @note	Blend states are immutable. Thread safe.
	 */
	class CM_EXPORT BlendState : public Resource
	{
	public:
		virtual ~BlendState() {}

		/**
		 * @brief	Alpha to coverage allows you to perform blending without needing to worry about order of
		 * 			rendering like regular blending does. It requires multi-sampling to be active in order to
		 * 			work, and you need to supply an alpha texture that determines object transparency.
		 *
		 *			Blending is then performed by only using sub-samples covered by the alpha texture for the current
		 *			pixel and combining them with sub-samples previously stored. 
		 *			
		 *			Be aware this is a limited technique only useful for certain situations. Unless you are having performance
		 *			problems use regular blending.
		 */
		bool getAlphaToCoverageEnabled() const { return mData.alphaToCoverageEnable; }

		/**
		 * @brief	When not set, only the first render target blend descriptor will be used for all
		 * 			render targets. If set each render target will use its own blend descriptor.
		 */
		bool getIndependantBlendEnable() const { return mData.independantBlendEnable; }

		/**
		 * @brief	Queries is blending enabled for the specified render target. Blending
		 * 			allows you to combine the color from current and previous pixel based on
		 * 			some value.
		 */
		bool getBlendEnabled(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines what should the source blend factor be. This value determines
		 * 			what will the color being generated currently be multiplied by.
		 */
		BlendFactor getSrcBlend(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines what should the destination blend factor be. This value determines
		 * 			what will the color already in render target be multiplied by.
		 */
		BlendFactor getDstBlend(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines how are source and destination colors combined (after they are multiplied
		 * 			by their respective blend factors).
		 */
		BlendOperation getBlendOperation(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines what should the alpha source blend factor be. This value determines
		 * 			what will the alpha value being generated currently be multiplied by.
		 */
		BlendFactor getAlphaSrcBlend(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines what should the alpha destination blend factor be. This value determines
		 * 			what will the alpha value already in render target be multiplied by.
		 */
		BlendFactor getAlphaDstBlend(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Determines how are source and destination alpha values combined (after they are multiplied
		 * 			by their respective blend factors).
		 */
		BlendOperation getAlphaBlendOperation(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Render target write mask allows to choose which pixel components should the pixel shader
		 * 			output.
		 * 			
		 *			Only the first four bits are used. First bit representing red, second green, third blue and
		 *			fourth alpha value. Set bits means pixel shader will output those channels.
		 */
		UINT8 getRenderTargetWriteMask(UINT32 renderTargetIdx) const;

		/**
		 * @brief	Creates a new blend state using the specified blend state description structure.
		 */
		static HBlendState create(const BLEND_STATE_DESC& desc);

		/**
		 * @brief	Returns the default blend state that you may use
		 * 			when no other is available.
		 */
		static const BlendStatePtr& getDefault();

	protected:
		friend class RenderStateManager;

		/**
		 * @brief	Initializes the blend state. Must be called right after construction.
		 */
		virtual void initialize(const BLEND_STATE_DESC& desc);

		BLEND_STATE_DESC mData;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class BlendStateRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}
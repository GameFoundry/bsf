#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
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

	// TODO Low priority - Write doc explaining various states
	class CM_EXPORT BlendState : public Resource
	{
	public:
		virtual ~BlendState() {}

		bool getAlphaToCoverageEnabled() const { return mData.alphaToCoverageEnable; }
		bool getIndependantBlendEnable() const { return mData.independantBlendEnable; }

		bool getBlendEnabled(UINT32 renderTargetIdx) const;
		BlendFactor getSrcBlend(UINT32 renderTargetIdx) const;
		BlendFactor getDstBlend(UINT32 renderTargetIdx) const;
		BlendOperation getBlendOperation(UINT32 renderTargetIdx) const;
		BlendFactor getAlphaSrcBlend(UINT32 renderTargetIdx) const;
		BlendFactor getAlphaDstBlend(UINT32 renderTargetIdx) const;
		BlendOperation getAlphaBlendOperation(UINT32 renderTargetIdx) const;
		UINT8 getRenderTargetWriteMask(UINT32 renderTargetIdx) const;

		static HBlendState create(const BLEND_STATE_DESC& desc);

		/**
		 * @brief	Returns the default blend state;
		 */
		static const BlendStatePtr& getDefault();

	protected:
		friend class RenderStateManager;

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
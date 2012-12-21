#pragma once

#include "CmPrerequisites.h"
#include "CmIReflectable.h"
#include "CmCommon.h"

namespace CamelotEngine
{
	struct CM_EXPORT RENDER_TARGET_BLEND_STATE_DESC
	{
		RENDER_TARGET_BLEND_STATE_DESC()
			: blendEnable(false)
			, srcBlend(SBF_ONE)
			, dstBlend(SBF_ZERO)
			, blendOp(SBO_ADD)
			, srcBlendAlpha(SBF_ONE)
			, dstBlendAlpha(SBF_ZERO)
			, blendOpAlpha(SBO_ADD)
			, renderTargetWriteMask(0xFF)
		{ }

		bool blendEnable;
		SceneBlendFactor srcBlend;
		SceneBlendFactor dstBlend;
		SceneBlendOperation blendOp;
		SceneBlendFactor srcBlendAlpha;
		SceneBlendFactor dstBlendAlpha;
		SceneBlendOperation blendOpAlpha;
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
	class CM_EXPORT BlendState : public IReflectable
	{
	public:
		virtual ~BlendState() {}

		bool getAlphaToCoverageEnabled() const { return mData.alphaToCoverageEnable; }
		bool getIndependantBlendEnable() const { return mData.independantBlendEnable; }

		bool getBlendEnabled(UINT32 renderTargetIdx) const;
		SceneBlendFactor getSrcBlend(UINT32 renderTargetIdx) const;
		SceneBlendFactor getDstBlend(UINT32 renderTargetIdx) const;
		SceneBlendOperation getBlendOperation(UINT32 renderTargetIdx) const;
		SceneBlendFactor getAlphaSrcBlend(UINT32 renderTargetIdx) const;
		SceneBlendFactor getAlphaDstBlend(UINT32 renderTargetIdx) const;
		SceneBlendOperation getAlphaBlendOperation(UINT32 renderTargetIdx) const;
		UINT8 getRenderTargetWriteMask(UINT32 renderTargetIdx) const;

		static BlendStatePtr create(const BLEND_STATE_DESC& desc);

		/**
		 * @brief	Returns the default sampler state;
		 */
		static const BlendState& getDefault();

	private:
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
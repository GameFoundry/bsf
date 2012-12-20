#pragma once

#include "CmPrerequisites.h"
#include "CmCommon.h"

namespace CamelotEngine
{
	struct CM_EXPORT RENDER_TARGET_BLEND_STATE_DESC
	{
		bool blendEnable;
		SceneBlendFactor srcBlend;
		SceneBlendFactor dstBlend;
		SceneBlendOperation blendOp;
		SceneBlendFactor srcBlendAlpha;
		SceneBlendFactor dstBlendAlpha;
		SceneBlendOperation blendOpAlpha;
		UINT8 renderTargetWriteMask;
	};

	struct CM_EXPORT BLEND_STATE_DESC
	{
		bool alphaToCoverageEnable;
		bool independantBlendEnable;
		RENDER_TARGET_BLEND_STATE_DESC renderTargetDesc[CM_MAX_MULTIPLE_RENDER_TARGETS];
	};

	class CM_EXPORT BlendState
	{
	public:
		virtual ~BlendState() {}

		bool getAlphaToCoverageEnabled() const { return mData.alphaToCoverageEnable; }
		bool getIndependantBlendEnable() const { return mData.independantBlendEnable; }

		bool getBlendEnable(UINT32 renderTargetIdx) const;
		SceneBlendFactor getSrcBlend(UINT32 renderTargetIdx) const;
		SceneBlendFactor getDstBlend(UINT32 renderTargetIdx) const;
		SceneBlendOperation getBlendOperation(UINT32 renderTargetIdx) const;
		SceneBlendFactor getAlphaSrcBlend(UINT32 renderTargetIdx) const;
		SceneBlendFactor getAlphaDstBlend(UINT32 renderTargetIdx) const;
		SceneBlendOperation getAlphaBlendOperation(UINT32 renderTargetIdx) const;
		UINT8 getRenderTargetWriteMask(UINT32 renderTargetIdx) const;

		static BlendStatePtr create(const BLEND_STATE_DESC& desc);

	private:
		friend class RenderStateManager;

		virtual void initialize(const BLEND_STATE_DESC& desc);
		BLEND_STATE_DESC mData;
	};
}
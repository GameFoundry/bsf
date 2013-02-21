#include "CmBlendState.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmBlendStateRTTI.h"

namespace CamelotEngine
{
	void BlendState::initialize(const BLEND_STATE_DESC& desc)
	{
		mData = desc;

		Resource::initialize();
	}

	const BlendStatePtr& BlendState::getDefault()
	{
		return RenderStateManager::instance().getDefaultBlendState();
	}

	bool BlendState::getBlendEnabled(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendEnable;
	}

	SceneBlendFactor BlendState::getSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlend;
	}

	SceneBlendFactor BlendState::getDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].dstBlend;
	}

	SceneBlendOperation BlendState::getBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOp;
	}

	SceneBlendFactor BlendState::getAlphaSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlendAlpha;
	}

	SceneBlendFactor BlendState::getAlphaDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);
		
		return mData.renderTargetDesc[renderTargetIdx].dstBlendAlpha;
	}

	SceneBlendOperation BlendState::getAlphaBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOpAlpha;
	}

	UINT8 BlendState::getRenderTargetWriteMask(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < CM_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].renderTargetWriteMask;
	}

	BlendStateHandle BlendState::create(const BLEND_STATE_DESC& desc)
	{
		BlendStatePtr blendStatePtr = RenderStateManager::instance().createBlendState(desc);

		return static_resource_cast<BlendState>(Resource::_createResourceHandle(blendStatePtr));
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* BlendState::getRTTIStatic()
	{
		return BlendStateRTTI::instance();
	}

	RTTITypeBase* BlendState::getRTTI() const
	{
		return BlendState::getRTTIStatic();
	}
}
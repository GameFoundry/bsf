#include "BsBlendState.h"
#include "BsRenderStateManager.h"
#include "BsRenderAPI.h"
#include "BsBlendStateRTTI.h"
#include "BsResources.h"

namespace BansheeEngine
{
	BlendProperties::BlendProperties(const BLEND_STATE_DESC& desc)
		:mData(desc)
	{ }

	bool BlendProperties::getBlendEnabled(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendEnable;
	}

	BlendFactor BlendProperties::getSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlend;
	}

	BlendFactor BlendProperties::getDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].dstBlend;
	}

	BlendOperation BlendProperties::getBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOp;
	}

	BlendFactor BlendProperties::getAlphaSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlendAlpha;
	}

	BlendFactor BlendProperties::getAlphaDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].dstBlendAlpha;
	}

	BlendOperation BlendProperties::getAlphaBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOpAlpha;
	}

	UINT8 BlendProperties::getRenderTargetWriteMask(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx >= 0 && renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].renderTargetWriteMask;
	}

	BlendStateCore::BlendStateCore(const BLEND_STATE_DESC& desc)
		:mProperties(desc)
	{

	}

	const BlendProperties& BlendStateCore::getProperties() const
	{
		return mProperties;
	}

	const SPtr<BlendStateCore>& BlendStateCore::getDefault()
	{
		return RenderStateCoreManager::instance().getDefaultBlendState();
	}

	BlendState::BlendState(const BLEND_STATE_DESC& desc)
		:mProperties(desc)
	{ }

	SPtr<BlendStateCore> BlendState::getCore() const
	{
		return std::static_pointer_cast<BlendStateCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> BlendState::createCore() const
	{
		return RenderStateCoreManager::instance().createBlendStateInternal(mProperties.mData);
	}

	const BlendProperties& BlendState::getProperties() const
	{
		return mProperties;
	}

	const BlendStatePtr& BlendState::getDefault()
	{
		return RenderStateManager::instance().getDefaultBlendState();
	}

	BlendStatePtr BlendState::create(const BLEND_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createBlendState(desc);
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
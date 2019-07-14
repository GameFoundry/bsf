//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "RenderAPI/BsBlendState.h"
#include "Managers/BsRenderStateManager.h"
#include "RenderAPI/BsRenderAPI.h"
#include "Private/RTTI/BsBlendStateRTTI.h"
#include "Resources/BsResources.h"

namespace bs
{
	bool RENDER_TARGET_BLEND_STATE_DESC::operator == (const RENDER_TARGET_BLEND_STATE_DESC& rhs) const
	{
		return blendEnable == rhs.blendEnable &&
			srcBlend == rhs.srcBlend &&
			dstBlend == rhs.dstBlend &&
			blendOp == rhs.blendOp &&
			srcBlendAlpha == rhs.srcBlendAlpha &&
			dstBlendAlpha == rhs.dstBlendAlpha &&
			blendOpAlpha == rhs.blendOpAlpha &&
			renderTargetWriteMask == rhs.renderTargetWriteMask;
	}

	bool BLEND_STATE_DESC::operator == (const BLEND_STATE_DESC& rhs) const
	{
		bool equals = alphaToCoverageEnable == rhs.alphaToCoverageEnable &&
			independantBlendEnable == rhs.independantBlendEnable;

		if (equals)
		{
			for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
			{
				equals &= renderTargetDesc[i] == rhs.renderTargetDesc[i];
			}
		}

		return equals;
	}

	BlendProperties::BlendProperties(const BLEND_STATE_DESC& desc)
		:mData(desc), mHash(BlendState::generateHash(desc))
	{ }

	bool BlendProperties::getBlendEnabled(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendEnable;
	}

	BlendFactor BlendProperties::getSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlend;
	}

	BlendFactor BlendProperties::getDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].dstBlend;
	}

	BlendOperation BlendProperties::getBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOp;
	}

	BlendFactor BlendProperties::getAlphaSrcBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].srcBlendAlpha;
	}

	BlendFactor BlendProperties::getAlphaDstBlend(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].dstBlendAlpha;
	}

	BlendOperation BlendProperties::getAlphaBlendOperation(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].blendOpAlpha;
	}

	UINT8 BlendProperties::getRenderTargetWriteMask(UINT32 renderTargetIdx) const
	{
		assert(renderTargetIdx < BS_MAX_MULTIPLE_RENDER_TARGETS);

		return mData.renderTargetDesc[renderTargetIdx].renderTargetWriteMask;
	}

	BlendState::BlendState(const BLEND_STATE_DESC& desc)
		:mProperties(desc), mId(0)
	{ }

	BlendState::~BlendState()
	{

	}

	SPtr<ct::BlendState> BlendState::getCore() const
	{
		return std::static_pointer_cast<ct::BlendState>(mCoreSpecific);
	}

	SPtr<ct::CoreObject> BlendState::createCore() const
	{
		SPtr<ct::BlendState> core = ct::RenderStateManager::instance()._createBlendState(mProperties.mData);
		mId = core->getId(); // Accessing core from sim thread is okay here since core ID is immutable

		return core;
	}

	const BlendProperties& BlendState::getProperties() const
	{
		return mProperties;
	}

	const SPtr<BlendState>& BlendState::getDefault()
	{
		return RenderStateManager::instance().getDefaultBlendState();
	}

	SPtr<BlendState> BlendState::create(const BLEND_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createBlendState(desc);
	}

	UINT64 BlendState::generateHash(const BLEND_STATE_DESC& desc)
	{
		size_t hash = 0;
		bs_hash_combine(hash, desc.alphaToCoverageEnable);
		bs_hash_combine(hash, desc.independantBlendEnable);

		for (UINT32 i = 0; i < BS_MAX_MULTIPLE_RENDER_TARGETS; i++)
		{
			bs_hash_combine(hash, desc.renderTargetDesc[i].blendEnable);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].srcBlend);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].dstBlend);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].blendOp);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].srcBlendAlpha);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].dstBlendAlpha);
			bs_hash_combine(hash, (UINT32)desc.renderTargetDesc[i].blendOpAlpha);
			bs_hash_combine(hash, desc.renderTargetDesc[i].renderTargetWriteMask);
		}

		return (UINT64)hash;
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

	namespace ct
	{
	BlendState::BlendState(const BLEND_STATE_DESC& desc, UINT32 id)
		:mProperties(desc), mId(id)
	{

	}

	BlendState::~BlendState()
	{

	}

	void BlendState::initialize()
	{
		// Since we cache states it's possible this object was already initialized
		// (i.e. multiple sim-states can share a single core-state)
		if (isInitialized())
			return;

		createInternal();
		CoreObject::initialize();
	}

	const BlendProperties& BlendState::getProperties() const
	{
		return mProperties;
	}

	SPtr<BlendState> BlendState::create(const BLEND_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createBlendState(desc);
	}

	const SPtr<BlendState>& BlendState::getDefault()
	{
		return RenderStateManager::instance().getDefaultBlendState();
	}
	}
}

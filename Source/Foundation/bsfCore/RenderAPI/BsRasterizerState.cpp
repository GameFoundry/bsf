//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "RenderAPI/BsRasterizerState.h"
#include "Managers/BsRenderStateManager.h"
#include "RenderAPI/BsRenderAPI.h"
#include "Private/RTTI/BsRasterizerStateRTTI.h"
#include "Resources/BsResources.h"

namespace bs
{
	bool RASTERIZER_STATE_DESC::operator == (const RASTERIZER_STATE_DESC& rhs) const
	{
		return polygonMode == rhs.polygonMode &&
			cullMode == rhs.cullMode &&
			depthBias == rhs.depthBias &&
			depthBiasClamp == rhs.depthBiasClamp &&
			slopeScaledDepthBias == rhs.slopeScaledDepthBias &&
			depthClipEnable == rhs.depthClipEnable &&
			scissorEnable == rhs.scissorEnable &&
			multisampleEnable == rhs.multisampleEnable &&
			antialiasedLineEnable == rhs.antialiasedLineEnable;
	}

	RasterizerProperties::RasterizerProperties(const RASTERIZER_STATE_DESC& desc)
		:mData(desc), mHash(RasterizerState::generateHash(desc))
	{ }

	RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc)
		: mProperties(desc), mId(0)
	{

	}

	RasterizerState::~RasterizerState()
	{

	}

	SPtr<ct::RasterizerState> RasterizerState::getCore() const
	{
		return std::static_pointer_cast<ct::RasterizerState>(mCoreSpecific);
	}

	SPtr<ct::CoreObject> RasterizerState::createCore() const
	{
		SPtr<ct::RasterizerState> core = ct::RenderStateManager::instance()._createRasterizerState(mProperties.mData);
		mId = core->getId(); // Accessing core from sim thread is okay here since core ID is immutable

		return core;
	}

	const RasterizerProperties& RasterizerState::getProperties() const
	{
		return mProperties;
	}

	const SPtr<RasterizerState>& RasterizerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultRasterizerState();
	}

	SPtr<RasterizerState> RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createRasterizerState(desc);
	}

	UINT64 RasterizerState::generateHash(const RASTERIZER_STATE_DESC& desc)
	{
		size_t hash = 0;
		bs_hash_combine(hash, (UINT32)desc.polygonMode);
		bs_hash_combine(hash, (UINT32)desc.cullMode);
		bs_hash_combine(hash, desc.depthBias);
		bs_hash_combine(hash, desc.depthBiasClamp);
		bs_hash_combine(hash, desc.slopeScaledDepthBias);
		bs_hash_combine(hash, desc.depthClipEnable);
		bs_hash_combine(hash, desc.scissorEnable);
		bs_hash_combine(hash, desc.multisampleEnable);
		bs_hash_combine(hash, desc.antialiasedLineEnable);

		return (UINT64)hash;
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* RasterizerState::getRTTIStatic()
	{
		return RasterizerStateRTTI::instance();
	}

	RTTITypeBase* RasterizerState::getRTTI() const
	{
		return RasterizerState::getRTTIStatic();
	}

	namespace ct
	{
	RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc, UINT32 id)
		: mProperties(desc), mId(id)
	{

	}

	RasterizerState::~RasterizerState()
	{

	}

	void RasterizerState::initialize()
	{
		// Since we cache states it's possible this object was already initialized
		// (i.e. multiple sim-states can share a single core-state)
		if (isInitialized())
			return;

		createInternal();
		CoreObject::initialize();
	}

	const RasterizerProperties& RasterizerState::getProperties() const
	{
		return mProperties;
	}

	SPtr<RasterizerState> RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createRasterizerState(desc);
	}

	const SPtr<RasterizerState>& RasterizerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultRasterizerState();
	}
	}
}

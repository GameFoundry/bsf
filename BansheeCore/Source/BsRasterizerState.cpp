#include "BsRasterizerState.h"
#include "BsRenderStateManager.h"
#include "BsRenderAPI.h"
#include "BsRasterizerStateRTTI.h"
#include "BsResources.h"

namespace BansheeEngine
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

	RasterizerStateCore::RasterizerStateCore(const RASTERIZER_STATE_DESC& desc)
		: mProperties(desc)
	{

	}

	RasterizerStateCore::~RasterizerStateCore()
	{
		RenderStateCoreManager::instance().notifyRasterizerStateDestroyed(mProperties.mData);
	}

	const RasterizerProperties& RasterizerStateCore::getProperties() const
	{
		return mProperties;
	}

	const SPtr<RasterizerStateCore>& RasterizerStateCore::getDefault()
	{
		return RenderStateCoreManager::instance().getDefaultRasterizerState();
	}

	RasterizerState::RasterizerState(const RASTERIZER_STATE_DESC& desc)
		: mProperties(desc)
	{

	}

	RasterizerState::~RasterizerState()
	{
		RenderStateManager::instance().notifyRasterizerStateDestroyed(mProperties.mData);
	}

	SPtr<RasterizerStateCore> RasterizerState::getCore() const
	{
		return std::static_pointer_cast<RasterizerStateCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> RasterizerState::createCore() const
	{
		return RenderStateCoreManager::instance().createRasterizerStateInternal(mProperties.mData);
	}

	const RasterizerProperties& RasterizerState::getProperties() const
	{
		return mProperties;
	}

	const RasterizerStatePtr& RasterizerState::getDefault()
	{
		return RenderStateManager::instance().getDefaultRasterizerState();
	}

	RasterizerStatePtr RasterizerState::create(const RASTERIZER_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createRasterizerState(desc);
	}

	UINT64 RasterizerState::generateHash(const RASTERIZER_STATE_DESC& desc)
	{
		size_t hash = 0;
		hash_combine(hash, (UINT32)desc.polygonMode);
		hash_combine(hash, (UINT32)desc.cullMode);
		hash_combine(hash, desc.depthBias);
		hash_combine(hash, desc.depthBiasClamp);
		hash_combine(hash, desc.slopeScaledDepthBias);
		hash_combine(hash, desc.depthClipEnable);
		hash_combine(hash, desc.scissorEnable);
		hash_combine(hash, desc.multisampleEnable);
		hash_combine(hash, desc.antialiasedLineEnable);

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
}
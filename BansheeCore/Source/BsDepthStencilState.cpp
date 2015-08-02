#include "BsDepthStencilState.h"
#include "BsRenderStateManager.h"
#include "BsRenderAPI.h"
#include "BsDepthStencilStateRTTI.h"
#include "BsException.h"
#include "BsResources.h"

namespace BansheeEngine
{
	bool DEPTH_STENCIL_STATE_DESC::operator == (const DEPTH_STENCIL_STATE_DESC& rhs) const
	{
		return depthReadEnable == rhs.depthReadEnable &&
			depthWriteEnable == rhs.depthWriteEnable &&
			depthComparisonFunc == rhs.depthComparisonFunc &&
			stencilEnable == rhs.stencilEnable &&
			stencilReadMask == rhs.stencilReadMask &&
			stencilWriteMask == rhs.stencilWriteMask &&
			frontStencilFailOp == rhs.frontStencilFailOp &&
			frontStencilZFailOp == rhs.frontStencilZFailOp &&
			frontStencilPassOp == rhs.frontStencilPassOp &&
			frontStencilComparisonFunc == rhs.frontStencilComparisonFunc &&
			backStencilFailOp == rhs.backStencilFailOp &&
			backStencilZFailOp == rhs.backStencilZFailOp &&
			backStencilPassOp == rhs.backStencilPassOp &&
			backStencilComparisonFunc == rhs.backStencilComparisonFunc;
	}

	DepthStencilProperties::DepthStencilProperties(const DEPTH_STENCIL_STATE_DESC& desc)
		:mData(desc), mHash(DepthStencilState::generateHash(desc))
	{

	}

	DepthStencilStateCore::DepthStencilStateCore(const DEPTH_STENCIL_STATE_DESC& desc)
		: mProperties(desc)
	{

	}

	DepthStencilStateCore::~DepthStencilStateCore()
	{
		RenderStateCoreManager::instance().notifyDepthStencilStateDestroyed(mProperties.mData);
	}

	const DepthStencilProperties& DepthStencilStateCore::getProperties() const
	{
		return mProperties;
	}

	const SPtr<DepthStencilStateCore>& DepthStencilStateCore::getDefault()
	{
		return RenderStateCoreManager::instance().getDefaultDepthStencilState();
	}

	DepthStencilState::DepthStencilState(const DEPTH_STENCIL_STATE_DESC& desc)
		:mProperties(desc)
	{

	}

	DepthStencilState::~DepthStencilState()
	{
		RenderStateManager::instance().notifyDepthStencilStateDestroyed(mProperties.mData);
	}

	SPtr<DepthStencilStateCore> DepthStencilState::getCore() const
	{
		return std::static_pointer_cast<DepthStencilStateCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> DepthStencilState::createCore() const
	{
		return RenderStateCoreManager::instance().createDepthStencilStateInternal(mProperties.mData);
	}

	const DepthStencilStatePtr& DepthStencilState::getDefault()
	{
		return RenderStateManager::instance().getDefaultDepthStencilState();
	}

	const DepthStencilProperties& DepthStencilState::getProperties() const
	{
		return mProperties;
	}

	DepthStencilStatePtr DepthStencilState::create(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createDepthStencilState(desc);
	}

	UINT64 DepthStencilState::generateHash(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		size_t hash = 0;
		hash_combine(hash, desc.depthReadEnable);
		hash_combine(hash, desc.depthWriteEnable);
		hash_combine(hash, (UINT32)desc.depthComparisonFunc);
		hash_combine(hash, desc.stencilEnable);
		hash_combine(hash, desc.stencilReadMask);
		hash_combine(hash, desc.stencilWriteMask);
		hash_combine(hash, (UINT32)desc.frontStencilFailOp);
		hash_combine(hash, (UINT32)desc.frontStencilZFailOp);
		hash_combine(hash, (UINT32)desc.frontStencilPassOp);
		hash_combine(hash, (UINT32)desc.frontStencilComparisonFunc);
		hash_combine(hash, (UINT32)desc.backStencilFailOp);
		hash_combine(hash, (UINT32)desc.backStencilZFailOp);
		hash_combine(hash, (UINT32)desc.backStencilPassOp);
		hash_combine(hash, (UINT32)desc.backStencilComparisonFunc);

		return (UINT64)hash;
	}

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* DepthStencilState::getRTTIStatic()
	{
		return DepthStencilStateRTTI::instance();
	}

	RTTITypeBase* DepthStencilState::getRTTI() const
	{
		return DepthStencilState::getRTTIStatic();
	}
}
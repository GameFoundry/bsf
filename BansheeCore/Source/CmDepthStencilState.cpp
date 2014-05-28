#include "CmDepthStencilState.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmDepthStencilStateRTTI.h"
#include "CmException.h"
#include "CmResources.h"

namespace BansheeEngine
{
	void DepthStencilState::initialize(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		mData = desc;

		Resource::initialize();
	}

	const DepthStencilStatePtr& DepthStencilState::getDefault()
	{
		return RenderStateManager::instance().getDefaultDepthStencilState();
	}

	HDepthStencilState DepthStencilState::create(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		DepthStencilStatePtr depthStencilPtr = RenderStateManager::instance().createDepthStencilState(desc);

		return static_resource_cast<DepthStencilState>(gResources()._createResourceHandle(depthStencilPtr));
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
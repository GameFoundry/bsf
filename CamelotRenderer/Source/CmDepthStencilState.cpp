#include "CmDepthStencilState.h"
#include "CmRenderStateManager.h"
#include "CmRenderSystem.h"
#include "CmDepthStencilStateRTTI.h"
#include "CmException.h"

namespace CamelotEngine
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

	DepthStencilStatePtr DepthStencilState::create(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		return RenderStateManager::instance().createDepthStencilState(desc);
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
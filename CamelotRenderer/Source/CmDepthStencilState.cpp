#include "CmDepthStencilState.h"
#include "CmRenderStateManager.h"
#include "CmDepthStencilStateRTTI.h"
#include "CmException.h"

namespace CamelotEngine
{
	void DepthStencilState::initialize(const DEPTH_STENCIL_STATE_DESC& desc)
	{
		mData = desc;
	}

	const DepthStencilStatePtr& DepthStencilState::getDefault()
	{
		static DepthStencilStatePtr depthStencilState = RenderStateManager::instance().createDepthStencilState(DEPTH_STENCIL_STATE_DESC());

		return depthStencilState;
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
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

	const DepthStencilState& DepthStencilState::getDefault()
	{
		static DepthStencilState depthStencilState;
		static bool initialized = false;

		if(!initialized)
		{
			depthStencilState.initialize(DEPTH_STENCIL_STATE_DESC());
			initialized = true;
		}

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
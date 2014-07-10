//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsDepthStencilState.h"
#include "BsRenderStateManager.h"
#include "BsRenderSystem.h"
#include "BsDepthStencilStateRTTI.h"
#include "BsException.h"
#include "BsResources.h"

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
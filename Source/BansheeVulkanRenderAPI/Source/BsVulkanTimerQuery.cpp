//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanTimerQuery.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	VulkanTimerQuery::VulkanTimerQuery(VulkanDevice& device)
	{
		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Query);
	}

	VulkanTimerQuery::~VulkanTimerQuery()
	{
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Query);
	}

	void VulkanTimerQuery::begin(const SPtr<CommandBuffer>& cb)
	{
		setActive(true);
	}

	void VulkanTimerQuery::end(const SPtr<CommandBuffer>& cb)
	{

	}

	bool VulkanTimerQuery::isReady() const
	{
		return false;
	}

	float VulkanTimerQuery::getTimeMs()
	{
		return 0.0f;
	}
}
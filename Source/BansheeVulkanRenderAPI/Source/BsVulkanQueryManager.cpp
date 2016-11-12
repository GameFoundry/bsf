//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanQueryManager.h"
#include "BsVulkanEventQuery.h"
#include "BsVulkanTimerQuery.h"
#include "BsVulkanOcclusionQuery.h"
#include "BsVulkanRenderAPI.h"
#include "BsVulkanDevice.h"

namespace BansheeEngine
{
	VulkanQueryManager::VulkanQueryManager(VulkanRenderAPI& rapi)
		:mRenderAPI(rapi)
	{ }

	VulkanQueryManager::~VulkanQueryManager()
	{
		
	}

	SPtr<EventQuery> VulkanQueryManager::createEventQuery(UINT32 deviceIdx) const
	{
		SPtr<VulkanDevice> device = mRenderAPI._getDevice(deviceIdx);

		SPtr<EventQuery> query = SPtr<VulkanEventQuery>(bs_new<VulkanEventQuery>(*device),
			&QueryManager::deleteEventQuery, StdAlloc<VulkanEventQuery>());
		mEventQueries.push_back(query.get());

		return query;
	}

	SPtr<TimerQuery> VulkanQueryManager::createTimerQuery(UINT32 deviceIdx) const
	{
		SPtr<VulkanDevice> device = mRenderAPI._getDevice(deviceIdx);

		SPtr<TimerQuery> query = SPtr<VulkanTimerQuery>(bs_new<VulkanTimerQuery>(device),
			&QueryManager::deleteTimerQuery, StdAlloc<VulkanTimerQuery>());
		mTimerQueries.push_back(query.get());

		return query;
	}

	SPtr<OcclusionQuery> VulkanQueryManager::createOcclusionQuery(bool binary, UINT32 deviceIdx) const
	{
		SPtr<VulkanDevice> device = mRenderAPI._getDevice(deviceIdx);

		SPtr<OcclusionQuery> query = SPtr<VulkanOcclusionQuery>(bs_new<VulkanOcclusionQuery>(*device, binary),
			&QueryManager::deleteOcclusionQuery, StdAlloc<VulkanOcclusionQuery>());
		mOcclusionQueries.push_back(query.get());

		return query;
	}

	VulkanEvent::VulkanEvent(VulkanResourceManager* owner)
		:VulkanResource(owner, false)
	{
		VkDevice vkDevice = owner->getDevice().getLogical();

		VkEventCreateInfo eventCI;
		eventCI.sType = VK_STRUCTURE_TYPE_EVENT_CREATE_INFO;
		eventCI.pNext = nullptr;
		eventCI.flags = 0;

		VkResult result = vkCreateEvent(vkDevice, &eventCI, gVulkanAllocator, &mEvent);
		assert(result == VK_SUCCESS);
	}

	VulkanEvent::~VulkanEvent()
	{
		VkDevice vkDevice = mOwner->getDevice().getLogical();
		vkDestroyEvent(vkDevice, mEvent, gVulkanAllocator);
	}

	bool VulkanEvent::isSignaled() const
	{
		VkDevice vkDevice = mOwner->getDevice().getLogical();
		return vkGetEventStatus(vkDevice, mEvent) == VK_EVENT_SET;
	}

	void VulkanEvent::reset()
	{
		VkDevice vkDevice = mOwner->getDevice().getLogical();

		VkResult result = vkResetEvent(vkDevice, mEvent);
		assert(result == VK_SUCCESS);
	}
}
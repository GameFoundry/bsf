//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanEventQuery.h"
#include "BsVulkanDevice.h"
#include "BsVulkanCommandBuffer.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	VulkanEventQuery::VulkanEventQuery(VulkanDevice& device)
		:mDevice(device), mEvent(nullptr)
	{
		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Query);
	}

	VulkanEventQuery::~VulkanEventQuery()
	{
		if (mEvent != nullptr)
			mEvent->destroy();

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Query);
	}

	void VulkanEventQuery::begin(const SPtr<CommandBuffer>& cb)
	{
		if (mEvent != nullptr)
		{
			if (mEvent->isBound())
			{
				// Clear current event and create a new one
				mEvent->destroy();
				mEvent = mDevice.getResourceManager().create<VulkanEvent>();
			}
			else
			{
				// Re-use existing event
				mEvent->reset();
			}
		}
		else // Create new event
			mEvent = mDevice.getResourceManager().create<VulkanEvent>();

		VulkanCommandBuffer* vulkanCB;
		if (cb != nullptr)
			vulkanCB = static_cast<VulkanCommandBuffer*>(cb.get());
		else
			vulkanCB = static_cast<VulkanCommandBuffer*>(gVulkanRenderAPI()._getMainCommandBuffer());

		VulkanCmdBuffer* internalCB = vulkanCB->getInternal(); 
		internalCB->registerResource(mEvent, VulkanUseFlag::Read);

		VkCommandBuffer vkCB = internalCB->getHandle();
		vkCmdSetEvent(vkCB, mEvent->getHandle(), VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

		setActive(true);
	}

	bool VulkanEventQuery::isReady() const
	{
		if (mEvent == nullptr)
			return false;

		return mEvent->isSignaled();
	}
}
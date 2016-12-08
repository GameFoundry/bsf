//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanQueue.h"
#include "BsVulkanCommandBuffer.h"

namespace bs
{
	VulkanQueue::VulkanQueue(VulkanDevice& device, VkQueue queue, GpuQueueType type, UINT32 index)
		:mDevice(device), mQueue(queue), mType(type), mIndex(index), mLastCommandBuffer(nullptr)
	{
		for (UINT32 i = 0; i < BS_MAX_UNIQUE_QUEUES; i++)
			mSubmitDstWaitMask[i] = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	}

	bool VulkanQueue::isExecuting() const
	{
		if (mLastCommandBuffer == nullptr)
			return false;

		return mLastCommandBuffer->isSubmitted();
	}

	void VulkanQueue::submit(VulkanCmdBuffer* cmdBuffer, VkSemaphore* waitSemaphores, UINT32 semaphoresCount)
	{
		VkCommandBuffer vkCmdBuffer = cmdBuffer->getHandle();
		VkSemaphore semaphore = cmdBuffer->getSemaphore();

		VkSubmitInfo submitInfo;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &vkCmdBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &semaphore;
		submitInfo.waitSemaphoreCount = semaphoresCount;

		if (semaphoresCount > 0)
		{
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = mSubmitDstWaitMask;
		}
		else
		{
			submitInfo.pWaitSemaphores = nullptr;
			submitInfo.pWaitDstStageMask = nullptr;
		}

		VkResult result = vkQueueSubmit(mQueue, 1, &submitInfo, cmdBuffer->getFence());
		assert(result == VK_SUCCESS);

		mLastCommandBuffer = cmdBuffer;
	}

	void VulkanQueue::waitIdle() const
	{
		VkResult result = vkQueueWaitIdle(mQueue);
		assert(result == VK_SUCCESS);
	}
}
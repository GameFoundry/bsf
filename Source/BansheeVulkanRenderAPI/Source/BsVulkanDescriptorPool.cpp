//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanDescriptorPool.h"
#include "BsVulkanDevice.h"

namespace BansheeEngine
{
	VulkanDescriptorPool::VulkanDescriptorPool(VulkanDevice& device)
		:mDevice(device)
	{
		const VkPhysicalDeviceLimits& limits = device.getDeviceProperties().limits;

		VkDescriptorPoolSize poolSizes[4];
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[0].descriptorCount = std::min(limits.maxPerStageDescriptorSampledImages, sMaxSampledImages);

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[1].descriptorCount = std::min(limits.maxPerStageDescriptorUniformBuffers, sMaxUniformBuffers);

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSizes[2].descriptorCount = std::min(limits.maxPerStageDescriptorStorageImages, sMaxImages);

		poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[3].descriptorCount = std::min(limits.maxPerStageDescriptorStorageBuffers, sMaxBuffers);

		VkDescriptorPoolCreateInfo poolCI;
		poolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolCI.pNext = nullptr;
		poolCI.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolCI.maxSets = std::min(limits.maxBoundDescriptorSets, sMaxSets);
		poolCI.poolSizeCount = sizeof(poolSizes)/sizeof(poolSizes[0]);
		poolCI.pPoolSizes = poolSizes;

		VkResult result = vkCreateDescriptorPool(mDevice.getLogical(), &poolCI, gVulkanAllocator, &mPool);
		assert(result == VK_SUCCESS);
	}

	VulkanDescriptorPool::~VulkanDescriptorPool()
	{
		vkDestroyDescriptorPool(mDevice.getLogical(), mPool, gVulkanAllocator);
	}
}
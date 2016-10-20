//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanDevice.h"

namespace BansheeEngine
{
	VulkanDevice::VulkanDevice(VkPhysicalDevice device)
		:mPhysicalDevice(device), mLogicalDevice(nullptr), mQueueInfos{}
	{
		vkGetPhysicalDeviceProperties(device, &mDeviceProperties);
		vkGetPhysicalDeviceFeatures(device, &mDeviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(device, &mMemoryProperties);

		uint32_t numQueueFamilies;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, nullptr);

		Vector<VkQueueFamilyProperties> queueFamilyProperties(numQueueFamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, queueFamilyProperties.data());

		// Create queues
		const float defaultQueuePriorities[BS_MAX_VULKAN_QUEUES_PER_TYPE] = { 0.0f };
		Vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		auto populateQueueInfo = [&](VulkanQueueType type, uint32_t familyIdx)
		{
			queueCreateInfos.push_back(VkDeviceQueueCreateInfo());

			VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.back();
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueFamilyIndex = familyIdx;
			createInfo.queueCount = std::min(queueFamilyProperties[familyIdx].queueCount, (uint32_t)BS_MAX_VULKAN_QUEUES_PER_TYPE);
			createInfo.pQueuePriorities = defaultQueuePriorities;

			mQueueInfos[type].familyIdx = familyIdx;
			mQueueInfos[type].queues.resize(createInfo.queueCount);
		};

		// Look for dedicated compute queues
		for (UINT32 i = 0; i < (UINT32)queueFamilyProperties.size(); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			{
				populateQueueInfo(VQT_COMPUTE, i);
				break;
			}
		}

		// Look for dedicated upload queues
		for (UINT32 i = 0; i < (UINT32)queueFamilyProperties.size(); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) &&
				((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
				((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				populateQueueInfo(VQT_UPLOAD, i);
				break;
			}
		}

		// Looks for graphics queues
		for (UINT32 i = 0; i < (UINT32)queueFamilyProperties.size(); i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				populateQueueInfo(VQT_GRAPHICS, i);
				break;
			}
		}

		// Create logical device
		const char* extensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		uint32_t numExtensions = sizeof(extensions) / sizeof(extensions[0]);

		VkDeviceCreateInfo deviceInfo;
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceInfo.pNext = nullptr;
		deviceInfo.flags = 0;
		deviceInfo.queueCreateInfoCount = (uint32_t)queueCreateInfos.size();
		deviceInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceInfo.pEnabledFeatures = &mDeviceFeatures;
		deviceInfo.enabledExtensionCount = numExtensions;
		deviceInfo.ppEnabledExtensionNames = extensions;
		deviceInfo.enabledLayerCount = 0;
		deviceInfo.ppEnabledLayerNames = nullptr;

		VkResult result = vkCreateDevice(device, &deviceInfo, gVulkanAllocator, &mLogicalDevice);
		assert(result == VK_SUCCESS);

		// Retrieve queues
		for(UINT32 i = 0; i < VQT_COUNT; i++)
		{
			UINT32 numQueues = (UINT32)mQueueInfos[i].queues.size();
			for(UINT32 j = 0; j < numQueues; j++)
				vkGetDeviceQueue(mLogicalDevice, mQueueInfos[i].familyIdx, j, &mQueueInfos[i].queues[j]);
		}
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDeviceWaitIdle(mLogicalDevice);
		vkDestroyDevice(mLogicalDevice, gVulkanAllocator);
	}
}
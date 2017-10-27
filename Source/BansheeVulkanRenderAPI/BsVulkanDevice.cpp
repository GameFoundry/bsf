//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanDevice.h"
#include "BsVulkanQueue.h"
#include "BsVulkanCommandBuffer.h"
#include "Managers/BsVulkanDescriptorManager.h"
#include "Managers/BsVulkanQueryManager.h"

namespace bs { namespace ct
{
	VulkanDevice::VulkanDevice(VkPhysicalDevice device, UINT32 deviceIdx)
		:mPhysicalDevice(device), mLogicalDevice(nullptr), mIsPrimary(false), mDeviceIdx(deviceIdx), mQueueInfos()
	{
		// Set to default
		for (UINT32 i = 0; i < GQT_COUNT; i++)
			mQueueInfos[i].familyIdx = (UINT32)-1;

		vkGetPhysicalDeviceProperties(device, &mDeviceProperties);
		vkGetPhysicalDeviceFeatures(device, &mDeviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(device, &mMemoryProperties);

		uint32_t numQueueFamilies;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, nullptr);

		Vector<VkQueueFamilyProperties> queueFamilyProperties(numQueueFamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &numQueueFamilies, queueFamilyProperties.data());

		// Create queues
		const float defaultQueuePriorities[BS_MAX_QUEUES_PER_TYPE] = { 0.0f };
		Vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		auto populateQueueInfo = [&](GpuQueueType type, uint32_t familyIdx)
		{
			queueCreateInfos.push_back(VkDeviceQueueCreateInfo());

			VkDeviceQueueCreateInfo& createInfo = queueCreateInfos.back();
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueFamilyIndex = familyIdx;
			createInfo.queueCount = std::min(queueFamilyProperties[familyIdx].queueCount, (uint32_t)BS_MAX_QUEUES_PER_TYPE);
			createInfo.pQueuePriorities = defaultQueuePriorities;

			mQueueInfos[type].familyIdx = familyIdx;
			mQueueInfos[type].queues.resize(createInfo.queueCount, nullptr);
		};

		// Look for dedicated compute queues
		for (UINT32 i = 0; i < (UINT32)queueFamilyProperties.size(); i++)
		{
			if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
			{
				populateQueueInfo(GQT_COMPUTE, i);
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
				populateQueueInfo(GQT_UPLOAD, i);
				break;
			}
		}

		// Looks for graphics queues
		for (UINT32 i = 0; i < (UINT32)queueFamilyProperties.size(); i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				populateQueueInfo(GQT_GRAPHICS, i);
				break;
			}
		}

		// Create logical device
		const char* extensions[] = { 
			VK_KHR_SWAPCHAIN_EXTENSION_NAME,
			VK_KHR_MAINTENANCE1_EXTENSION_NAME
		};
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
		for(UINT32 i = 0; i < GQT_COUNT; i++)
		{
			UINT32 numQueues = (UINT32)mQueueInfos[i].queues.size();
			for (UINT32 j = 0; j < numQueues; j++)
			{
				VkQueue queue;
				vkGetDeviceQueue(mLogicalDevice, mQueueInfos[i].familyIdx, j, &queue);

				mQueueInfos[i].queues[j] = bs_new<VulkanQueue>(*this, queue, (GpuQueueType)i, j);
			}
		}

		// Create pools/managers
		mCommandBufferPool = bs_new<VulkanCmdBufferPool>(*this);
		mQueryPool = bs_new<VulkanQueryPool>(*this);
		mDescriptorManager = bs_new<VulkanDescriptorManager>(*this);
		mResourceManager = bs_new<VulkanResourceManager>(*this);
	}

	VulkanDevice::~VulkanDevice()
	{
		waitIdle();

		for (UINT32 i = 0; i < GQT_COUNT; i++)
		{
			UINT32 numQueues = (UINT32)mQueueInfos[i].queues.size();
			for (UINT32 j = 0; j < numQueues; j++)
			{
				mQueueInfos[i].queues[j]->refreshStates(true, true);
				bs_delete(mQueueInfos[i].queues[j]);
			}
		}

		bs_delete(mDescriptorManager);
		bs_delete(mQueryPool);
		bs_delete(mCommandBufferPool);

		// Needs to happen after query pool & command buffer pool shutdown, to ensure their resources are destroyed
		bs_delete(mResourceManager);
		
		vkDestroyDevice(mLogicalDevice, gVulkanAllocator);
	}

	void VulkanDevice::waitIdle() const
	{
		VkResult result = vkDeviceWaitIdle(mLogicalDevice);
		assert(result == VK_SUCCESS);
	}

	UINT32 VulkanDevice::getQueueMask(GpuQueueType type, UINT32 queueIdx) const
	{
		UINT32 numQueues = getNumQueues(type);
		if (numQueues == 0)
			return 0;

		UINT32 idMask = 0;
		UINT32 curIdx = queueIdx % numQueues;
		while (curIdx < BS_MAX_QUEUES_PER_TYPE)
		{
			idMask |= CommandSyncMask::getGlobalQueueMask(type, curIdx);
			curIdx += numQueues;
		}

		return idMask;
	}

	SurfaceFormat VulkanDevice::getSurfaceFormat(const VkSurfaceKHR& surface, bool gamma) const
	{
		uint32_t numFormats;
		VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &numFormats, nullptr);
		assert(result == VK_SUCCESS);
		assert(numFormats > 0);

		VkSurfaceFormatKHR* surfaceFormats = bs_stack_alloc<VkSurfaceFormatKHR>(numFormats);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(mPhysicalDevice, surface, &numFormats, surfaceFormats);
		assert(result == VK_SUCCESS);

		SurfaceFormat output;
		output.colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
		output.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		output.depthFormat = VK_FORMAT_D24_UNORM_S8_UINT;

		// If there is no preferred format, use standard RGBA
		if ((numFormats == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			if (gamma)
				output.colorFormat = VK_FORMAT_R8G8B8A8_SRGB;
			else
				output.colorFormat = VK_FORMAT_B8G8R8A8_UNORM;

			output.colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool foundFormat = false;

			VkFormat wantedFormatsUNORM[] =
			{
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_FORMAT_B8G8R8A8_UNORM,
				VK_FORMAT_A8B8G8R8_UNORM_PACK32,
				VK_FORMAT_A8B8G8R8_UNORM_PACK32,
				VK_FORMAT_R8G8B8_UNORM,
				VK_FORMAT_B8G8R8_UNORM
			};

			VkFormat wantedFormatsSRGB[] =
			{
				VK_FORMAT_R8G8B8A8_SRGB,
				VK_FORMAT_B8G8R8A8_SRGB,
				VK_FORMAT_A8B8G8R8_SRGB_PACK32,
				VK_FORMAT_A8B8G8R8_SRGB_PACK32,
				VK_FORMAT_R8G8B8_SRGB,
				VK_FORMAT_B8G8R8_SRGB
			};

			UINT32 numWantedFormats;
			VkFormat* wantedFormats;
			if (gamma)
			{
				numWantedFormats = sizeof(wantedFormatsSRGB) / sizeof(wantedFormatsSRGB[0]);
				wantedFormats = wantedFormatsSRGB;
			}
			else
			{
				numWantedFormats = sizeof(wantedFormatsUNORM) / sizeof(wantedFormatsUNORM[0]);
				wantedFormats = wantedFormatsUNORM;
			}

			for(UINT32 i = 0; i < numWantedFormats; i++)
			{
				for(UINT32 j = 0; j < numFormats; j++)
				{
					if(surfaceFormats[j].format == wantedFormats[i])
					{
						output.colorFormat = surfaceFormats[j].format;
						output.colorSpace = surfaceFormats[j].colorSpace;

						foundFormat = true;
						break;
					}
				}

				if (foundFormat)
					break;
			}

			// If we haven't found anything, fall back to first available
			if(!foundFormat)
			{
				output.colorFormat = surfaceFormats[0].format;
				output.colorSpace = surfaceFormats[0].colorSpace;

				if (gamma)
					LOGERR("Cannot find a valid sRGB format for a render window surface, falling back to a default format.");
			}
		}

		bs_stack_free(surfaceFormats);
		return output;
	}

	VkDeviceMemory VulkanDevice::allocateMemory(VkImage image, VkMemoryPropertyFlags flags)
	{
		VkMemoryRequirements memReq;
		vkGetImageMemoryRequirements(mLogicalDevice, image, &memReq);

		VkDeviceMemory memory = allocateMemory(memReq, flags);

		VkResult result = vkBindImageMemory(mLogicalDevice, image, memory, 0);
		assert(result == VK_SUCCESS);

		return memory;
	}

	VkDeviceMemory VulkanDevice::allocateMemory(VkBuffer buffer, VkMemoryPropertyFlags flags)
	{
		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(mLogicalDevice, buffer, &memReq);

		VkDeviceMemory memory = allocateMemory(memReq, flags);

		VkResult result = vkBindBufferMemory(mLogicalDevice, buffer, memory, 0);
		assert(result == VK_SUCCESS);

		return memory;
	}

	VkDeviceMemory VulkanDevice::allocateMemory(const VkMemoryRequirements& reqs, VkMemoryPropertyFlags flags)
	{
		VkMemoryAllocateInfo allocateInfo;
		allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfo.pNext = nullptr;
		allocateInfo.memoryTypeIndex = findMemoryType(reqs.memoryTypeBits, flags);
		allocateInfo.allocationSize = reqs.size;

		if (allocateInfo.memoryTypeIndex == (UINT32)-1)
			return VK_NULL_HANDLE;

		VkDeviceMemory memory;

		VkResult result = vkAllocateMemory(mLogicalDevice, &allocateInfo, gVulkanAllocator, &memory);
		assert(result == VK_SUCCESS);

		return memory;
	}

	void VulkanDevice::freeMemory(VkDeviceMemory memory)
	{
		vkFreeMemory(mLogicalDevice, memory, gVulkanAllocator);
	}

	uint32_t VulkanDevice::findMemoryType(uint32_t requirementBits, VkMemoryPropertyFlags wantedFlags)
	{
		for (uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; i++)
		{
			if (requirementBits & (1 << i))
			{
				if ((mMemoryProperties.memoryTypes[i].propertyFlags & wantedFlags) == wantedFlags)
					return i;
			}
		}

		return -1;
	}
}}
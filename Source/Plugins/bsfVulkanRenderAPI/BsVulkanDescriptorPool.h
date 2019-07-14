//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsVulkanPrerequisites.h"

namespace bs { namespace ct
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Pool that allocates and distributes Vulkan descriptor sets. */
	class VulkanDescriptorPool
	{
	public:
		VulkanDescriptorPool(VulkanDevice& device);
		~VulkanDescriptorPool();

		/** Returns a handle to the internal Vulkan descriptor pool. */
		VkDescriptorPool getHandle() const { return mPool; }

	private:
		static const UINT32 sMaxSets = 8192;
		static const UINT32 sMaxSampledImages = 4096;
		static const UINT32 sMaxImages = 2048;
		static const UINT32 sMaxSampledBuffers = 2048;
		static const UINT32 sMaxBuffers = 2048;
		static const UINT32 sMaxUniformBuffers = 2048;

		VulkanDevice& mDevice;
		VkDescriptorPool mPool;
	};

	/** @} */
}}

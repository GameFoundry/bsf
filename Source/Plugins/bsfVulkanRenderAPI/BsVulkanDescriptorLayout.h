//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsVulkanPrerequisites.h"

namespace bs { namespace ct
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Wrapper for the Vulkan descriptor layout object. */
	class VulkanDescriptorLayout
	{
	public:
		VulkanDescriptorLayout(VulkanDevice& device, VkDescriptorSetLayoutBinding* bindings, UINT32 numBindings);
		~VulkanDescriptorLayout();

		/** Returns a handle to the Vulkan set layout object. */
		VkDescriptorSetLayout getHandle() const { return mLayout; }

		/** Returns a hash value for the descriptor layout. */
		size_t getHash() const { return mHash; }

		/** Calculates a has value for the provided descriptor set layout bindings. */
		static size_t calculateHash(VkDescriptorSetLayoutBinding* bindings, UINT32 numBindings);

	protected:
		VulkanDevice& mDevice;
		VkDescriptorSetLayout mLayout;
		size_t mHash;
	};

	/** @} */
}}

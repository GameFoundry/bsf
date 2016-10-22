//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsPixelUtil.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Contains various helper methods for dealing with Vulkan. */
	class VulkanUtility
	{
	public:
		/** Converts between Banshee and Vulkan pixel format. */
		static VkFormat getPixelFormat(PixelFormat format, bool sRGB = false);

		/** Gets Vulkan flags representing the number of samples in an image. Sample count must be a power of 2. */
		static VkSampleCountFlagBits getSampleFlags(UINT32 numSamples);
	};

	/** @} */
}
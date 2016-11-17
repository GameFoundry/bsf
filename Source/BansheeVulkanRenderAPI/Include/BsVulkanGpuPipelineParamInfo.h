//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsGpuPipelineParamInfo.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Holds meta-data about a set of GPU parameters used by a single pipeline state. */
	class VulkanGpuPipelineParamInfo : public GpuPipelineParamInfoCore
	{
	public:
		VulkanGpuPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc);
		~VulkanGpuPipelineParamInfo();

	private:
	};

	/** @} */
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanGpuPipelineParamInfo.h"

namespace BansheeEngine
{
	VulkanGpuPipelineParamInfo::VulkanGpuPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc)
		:GpuPipelineParamInfoCore(desc)
	{
		// TODO - Will get called from main thread!
	}

	VulkanGpuPipelineParamInfo::~VulkanGpuPipelineParamInfo()
	{
		
	}
}
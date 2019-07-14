//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Managers/BsVulkanRenderStateManager.h"
#include "BsVulkanGpuPipelineState.h"
#include "BsVulkanGpuPipelineParamInfo.h"
#include "BsVulkanSamplerState.h"

namespace bs { namespace ct
{
	SPtr<SamplerState> VulkanRenderStateManager::createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
		GpuDeviceFlags deviceMask) const
	{
		SPtr<VulkanSamplerState> samplerState =
			bs_shared_ptr<VulkanSamplerState>(new (bs_alloc<VulkanSamplerState>()) VulkanSamplerState(desc, deviceMask));
		samplerState->_setThisPtr(samplerState);

		return samplerState;
	}

	SPtr<GraphicsPipelineState> VulkanRenderStateManager::_createGraphicsPipelineState(
		const PIPELINE_STATE_DESC& desc, GpuDeviceFlags deviceMask) const
	{
		SPtr<VulkanGraphicsPipelineState> pipelineState =
			bs_shared_ptr<VulkanGraphicsPipelineState>(new (bs_alloc<VulkanGraphicsPipelineState>())
			VulkanGraphicsPipelineState(desc, deviceMask));
		pipelineState->_setThisPtr(pipelineState);

		return pipelineState;
	}

	SPtr<ComputePipelineState> VulkanRenderStateManager::_createComputePipelineState(
		const SPtr<GpuProgram>& program, GpuDeviceFlags deviceMask) const
	{
		SPtr<VulkanComputePipelineState> pipelineState =
			bs_shared_ptr<VulkanComputePipelineState>(new (bs_alloc<VulkanComputePipelineState>())
														  VulkanComputePipelineState(program, deviceMask));
		pipelineState->_setThisPtr(pipelineState);

		return pipelineState;
	}

	SPtr<GpuPipelineParamInfo> VulkanRenderStateManager::_createPipelineParamInfo(
		const GPU_PIPELINE_PARAMS_DESC& desc, GpuDeviceFlags deviceMask) const
	{
		SPtr<VulkanGpuPipelineParamInfo> paramInfo =
			bs_shared_ptr<VulkanGpuPipelineParamInfo>(new (bs_alloc<VulkanGpuPipelineParamInfo>())
													  VulkanGpuPipelineParamInfo(desc, deviceMask));
		paramInfo->_setThisPtr(paramInfo);

		return paramInfo;
	}
}}

//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "Managers/BsRenderStateManager.h"

namespace bs { namespace ct
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/**	Handles creation of Vulkan pipeline states. */
	class VulkanRenderStateManager : public RenderStateManager
	{
	protected:
		/** @copydoc RenderStateManager::createSamplerStateInternal */
		SPtr<SamplerState> createSamplerStateInternal(const SAMPLER_STATE_DESC& desc,
			GpuDeviceFlags deviceMask) const override;

		/** @copydoc RenderStateManager::_createGraphicsPipelineState */
		SPtr<GraphicsPipelineState> _createGraphicsPipelineState(const PIPELINE_STATE_DESC& desc,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) const override;

		/** @copydoc RenderStateManager::_createComputePipelineState */
		SPtr<ComputePipelineState> _createComputePipelineState(const SPtr<GpuProgram>& program,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) const override;

		/** @copydoc RenderStateManager::_createPipelineParamInfo */
		SPtr<GpuPipelineParamInfo> _createPipelineParamInfo(const GPU_PIPELINE_PARAMS_DESC& desc,
			 GpuDeviceFlags deviceMask = GDF_DEFAULT) const override;
	};

	/** @} */
}}

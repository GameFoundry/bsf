//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"
#include "BsGpuPipelineState.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	 /** Wrapper around a Vulkan graphics pipeline that manages its usage and lifetime. */
	class VulkanPipeline : public VulkanResource
	{
	public:
		VulkanPipeline(VulkanResourceManager* owner, VkPipeline pipeline);
		~VulkanPipeline();

		/** Returns the internal handle to the Vulkan object. */
		VkPipeline getHandle() const { return mPipeline; }

	private:
		VkPipeline mPipeline;
	};

	/**	Vulkan implementation of a graphics pipeline state. */
	class VulkanGraphicsPipelineStateCore : public GraphicsPipelineStateCore
	{
	public:
		~VulkanGraphicsPipelineStateCore();

	protected:
		friend class VulkanRenderStateCoreManager;

		VulkanGraphicsPipelineStateCore(const PIPELINE_STATE_CORE_DESC& desc, GpuDeviceFlags deviceMask);

		/**	@copydoc GraphicsPipelineStateCore::initialize */
		void initialize() override;

		/** 
		 * Create a new Vulkan graphics pipeline. 
		 * 
		 * @param[in]	deviceIdx			Index of the device to create the pipeline for.
		 * @param[in]	framebuffer			Framebuffer object that defines the surfaces this pipeline will render to.
		 * @param[in]	readOnlyDepth		True if the pipeline is only allowed to read the depth buffer, without writes.
		 * @param[in]	drawOp				Type of geometry that will be drawn using the pipeline.
		 * @param[in]	vertexInputState	State describing inputs to the vertex program.
		 * @return							Vulkan graphics pipeline object.
		 */
		VkPipeline createPipeline(UINT32 deviceIdx, VulkanFramebuffer* framebuffer, bool readOnlyDepth,
								  DrawOperationType drawOp, VkPipelineVertexInputStateCreateInfo* vertexInputState);

		/** Contains pipeline data specific to a single Vulkan device. */
		struct PerDeviceData
		{
			VulkanDevice* device;
			VkPipelineLayout pipelineLayout;
		};

		VkPipelineShaderStageCreateInfo mShaderStageInfos[5];
		VkPipelineInputAssemblyStateCreateInfo mInputAssemblyInfo;
		VkPipelineTessellationStateCreateInfo mTesselationInfo;
		VkPipelineViewportStateCreateInfo mViewportInfo;
		VkPipelineRasterizationStateCreateInfo mRasterizationInfo;
		VkPipelineMultisampleStateCreateInfo mMultiSampleInfo;
		VkPipelineDepthStencilStateCreateInfo mDepthStencilInfo;
		VkPipelineColorBlendAttachmentState mAttachmentBlendStates[BS_MAX_MULTIPLE_RENDER_TARGETS];
		VkPipelineColorBlendStateCreateInfo mColorBlendStateInfo;
		VkPipelineDynamicStateCreateInfo mDynamicStateInfo;
		VkDynamicState mDynamicStates[3];
		VkGraphicsPipelineCreateInfo mPipelineInfo;

		GpuDeviceFlags mDeviceMask;
		PerDeviceData mPerDeviceData[BS_MAX_DEVICES];
	};

	/**	Vulkan implementation of a compute pipeline state. */
	class VulkanComputePipelineStateCore : public ComputePipelineStateCore
	{
	public:
		~VulkanComputePipelineStateCore();

	protected:
		friend class VulkanRenderStateCoreManager;

		VulkanComputePipelineStateCore(const SPtr<GpuProgramCore>& program, GpuDeviceFlags deviceMask);

		/**	@copydoc ComputePipelineStateCore::initialize */
		void initialize() override;

		/** Contains pipeline data specific to a single Vulkan device. */
		struct PerDeviceData
		{
			VulkanDevice* device;
			VulkanPipeline* pipeline;
		};

		GpuDeviceFlags mDeviceMask;
		PerDeviceData mPerDeviceData[BS_MAX_DEVICES];
	};

	/** @} */
}
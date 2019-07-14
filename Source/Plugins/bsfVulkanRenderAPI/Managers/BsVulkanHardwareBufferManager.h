//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "Managers/BsHardwareBufferManager.h"

namespace bs { namespace ct
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/**	Handles creation of Vulkan hardware buffers. */
	class VulkanHardwareBufferManager : public HardwareBufferManager
	{
	public:
		VulkanHardwareBufferManager();
		~VulkanHardwareBufferManager();

		/** Returns a buffer that can be used for buffer read operations when no other buffer is bound. */
		VulkanHardwareBuffer* getDummyReadBuffer() const { return mDummyReadBuffer; }

		/** Returns a buffer that can be used for buffer storage operations when no other buffer is bound. */
		VulkanHardwareBuffer* getDummyStorageBuffer() const { return mDummyStorageBuffer; }

		/** Returns a buffer that can be used for uniform storage when no other buffer is bound. */
		VulkanHardwareBuffer* getDummyUniformBuffer() const { return mDummyUniformBuffer; }

		/** Returns a buffer that can be used for structured storage when no other buffer is bound. */
		VulkanHardwareBuffer* getDummyStructuredBuffer() const { return mDummyStructuredBuffer; }

	protected:
		/** @copydoc HardwareBufferManager::createVertexBufferInternal */
		SPtr<VertexBuffer> createVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		/** @copydoc HardwareBufferManager::createIndexBufferInternal */
		SPtr<IndexBuffer> createIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		/** @copydoc HardwareBufferManager::createGpuParamBlockBufferInternal  */
		SPtr<GpuParamBlockBuffer> createGpuParamBlockBufferInternal(UINT32 size,
			GpuBufferUsage usage = GBU_DYNAMIC, GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		/** @copydoc HardwareBufferManager::createGpuBufferInternal(const GPU_BUFFER_DESC&, GpuDeviceFlags) */
		SPtr<GpuBuffer> createGpuBufferInternal(const GPU_BUFFER_DESC& desc,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		/** @copydoc HardwareBufferManager::createGpuBufferInternal(const GPU_BUFFER_DESC&, SPtr<HardwareBuffer>) */
		SPtr<GpuBuffer> createGpuBufferInternal(const GPU_BUFFER_DESC& desc,
			SPtr<HardwareBuffer> underlyingBuffer) override;

		/** @copydoc HardwareBufferManager::createGpuParamsInternal */
		SPtr<GpuParams> createGpuParamsInternal(const SPtr<GpuPipelineParamInfo>& paramInfo,
			GpuDeviceFlags deviceMask = GDF_DEFAULT) override;

		VulkanHardwareBuffer* mDummyReadBuffer;
		VulkanHardwareBuffer* mDummyStorageBuffer;
		VulkanHardwareBuffer* mDummyUniformBuffer;
		VulkanHardwareBuffer* mDummyStructuredBuffer;
	};

	/** @} */
}}

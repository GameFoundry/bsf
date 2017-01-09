//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsVulkanHardwareBufferManager.h"
#include "BsVulkanVertexBuffer.h"
#include "BsVulkanIndexBuffer.h"
#include "BsVulkanGpuBuffer.h"
#include "BsVulkanGpuParamBlockBuffer.h"
#include "BsVulkanGpuParams.h"
#include "BsVulkanHardwareBuffer.h"
#include "BsGpuParamDesc.h"

namespace bs { namespace ct
{
	VulkanHardwareBufferCoreManager::VulkanHardwareBufferCoreManager()
	{
		// Note: When multi-GPU is properly tested, make sure to create these textures on all GPUs
		mDummyReadBuffer = bs_new<VulkanHardwareBuffer>(
			VulkanHardwareBuffer::BT_GENERIC, BF_32X1F, GBU_STATIC, 16, GDF_DEFAULT);

		mDummyStorageBuffer = bs_new<VulkanHardwareBuffer>(
			VulkanHardwareBuffer::BT_STORAGE, BF_32X1F, GBU_STATIC, 16, GDF_DEFAULT);

		mDummyUniformBuffer = bs_new<VulkanHardwareBuffer>(
			VulkanHardwareBuffer::BT_UNIFORM, BF_UNKNOWN, GBU_STATIC, 16, GDF_DEFAULT);
	}

	VulkanHardwareBufferCoreManager::~VulkanHardwareBufferCoreManager()
	{
		bs_delete(mDummyReadBuffer);
		bs_delete(mDummyStorageBuffer);
		bs_delete(mDummyUniformBuffer);
	}

	VkBufferView VulkanHardwareBufferCoreManager::getDummyReadBufferView(UINT32 deviceIdx) const
	{
		return mDummyReadBuffer->getResource(deviceIdx)->getView();
	}

	VkBufferView VulkanHardwareBufferCoreManager::getDummyStorageBufferView(UINT32 deviceIdx) const
	{
		return mDummyStorageBuffer->getResource(deviceIdx)->getView();
	}

	VkBuffer VulkanHardwareBufferCoreManager::getDummyUniformBuffer(UINT32 deviceIdx) const
	{
		return mDummyUniformBuffer->getResource(deviceIdx)->getHandle();
	}

	SPtr<VertexBufferCore> VulkanHardwareBufferCoreManager::createVertexBufferInternal(const VERTEX_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		SPtr<VulkanVertexBufferCore> ret = bs_shared_ptr_new<VulkanVertexBufferCore>(desc, deviceMask);
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<IndexBufferCore> VulkanHardwareBufferCoreManager::createIndexBufferInternal(const INDEX_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		SPtr<VulkanIndexBufferCore> ret = bs_shared_ptr_new<VulkanIndexBufferCore>( desc, deviceMask);
		ret->_setThisPtr(ret);

		return ret;
	}

	SPtr<GpuParamBlockBufferCore> VulkanHardwareBufferCoreManager::createGpuParamBlockBufferInternal(UINT32 size,
		GpuParamBlockUsage usage, GpuDeviceFlags deviceMask)
	{
		VulkanGpuParamBlockBufferCore* paramBlockBuffer =
			new (bs_alloc<VulkanGpuParamBlockBufferCore>()) VulkanGpuParamBlockBufferCore(size, usage, deviceMask);

		SPtr<GpuParamBlockBufferCore> paramBlockBufferPtr = bs_shared_ptr<VulkanGpuParamBlockBufferCore>(paramBlockBuffer);
		paramBlockBufferPtr->_setThisPtr(paramBlockBufferPtr);

		return paramBlockBufferPtr;
	}

	SPtr<GpuBufferCore> VulkanHardwareBufferCoreManager::createGpuBufferInternal(const GPU_BUFFER_DESC& desc,
		GpuDeviceFlags deviceMask)
	{
		VulkanGpuBufferCore* buffer = new (bs_alloc<VulkanGpuBufferCore>()) VulkanGpuBufferCore(desc, deviceMask);

		SPtr<VulkanGpuBufferCore> bufferPtr = bs_shared_ptr<VulkanGpuBufferCore>(buffer);
		bufferPtr->_setThisPtr(bufferPtr);

		return bufferPtr;
	}

	SPtr<GpuParamsCore> VulkanHardwareBufferCoreManager::createGpuParamsInternal(
		const SPtr<GpuPipelineParamInfoCore>& paramInfo, GpuDeviceFlags deviceMask)
	{
		VulkanGpuParams* params = new (bs_alloc<VulkanGpuParams>()) VulkanGpuParams(paramInfo, deviceMask);
		SPtr<GpuParamsCore> paramsPtr = bs_shared_ptr<GpuParamsCore>(params);
		paramsPtr->_setThisPtr(paramsPtr);

		return paramsPtr;
	}
}}
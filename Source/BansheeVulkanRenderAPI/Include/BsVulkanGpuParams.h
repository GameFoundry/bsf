//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsGpuParams.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Vulkan implementation of GpuParams, containing resource descriptors for all shader stages. */
	class VulkanGpuParams : public GpuParamsCore
	{
	public:
		~VulkanGpuParams();

		/** @copydoc GpuParamsCore::setParamBlockBuffer(UINT32, UINT32, const ParamsBufferType&) */
		void setParamBlockBuffer(UINT32 set, UINT32 slot, const ParamsBufferType& paramBlockBuffer) override;

		/** @copydoc GpuParamsCore::setTexture */
		void setTexture(UINT32 set, UINT32 slot, const TextureType& texture) override;

		/** @copydoc GpuParamsCore::setLoadStoreTexture */
		void setLoadStoreTexture(UINT32 set, UINT32 slot, const TextureType& texture, 
			const TextureSurface& surface) override;

		/** @copydoc GpuParamsCore::setBuffer */
		void setBuffer(UINT32 set, UINT32 slot, const BufferType& buffer) override;

		/** @copydoc GpuParamsCore::setSamplerState */
		void setSamplerState(UINT32 set, UINT32 slot, const SamplerType& sampler) override;

		/** @copydoc GpuParamsCore::setLoadStoreSurface */
		void setLoadStoreSurface(UINT32 set, UINT32 slot, const TextureSurface& surface) override;

	protected:
		friend class VulkanHardwareBufferCoreManager;

		/** Information about a single descriptor set. */
		struct SetInfo
		{
			VkDescriptorSetLayout layout;
			VkDescriptorSet set;
		};

		VulkanGpuParams(const GPU_PARAMS_DESC& desc, GpuDeviceFlags deviceMask);

		SetInfo* mSets;
		UINT32 mNumSets;
	};

	/** @} */
}
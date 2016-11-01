//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsVulkanPrerequisites.h"
#include "BsVulkanResource.h"
#include "BsSamplerState.h"

namespace BansheeEngine
{
	/** @addtogroup Vulkan
	 *  @{
	 */

	/** Wrapper around a Vulkan sampler object that manages its usage and lifetime. */
	class VulkanSampler : public VulkanResource
	{
	public:
		VulkanSampler(VulkanResourceManager* owner, VkSampler sampler);
		~VulkanSampler();

		/** Returns the internal handle to the Vulkan object. */
		VkSampler getHandle() const { return mSampler; }

	private:
		VkSampler mSampler;
	};

	/**	Vulkan implementation of a sampler state. Wraps a Vulkan sampler object. */
	class VulkanSamplerStateCore : public SamplerStateCore
	{
	public:
		~VulkanSamplerStateCore();

		/** Gets the resource wrapping the sampler object. */
		VulkanSampler* getResource() const { return mSampler; }

	protected:
		friend class D3D11RenderStateCoreManager;

		VulkanSamplerStateCore(const SAMPLER_STATE_DESC& desc);

		/** @copydoc SamplerStateCore::createInternal */
		void createInternal() override;

		VulkanSampler* mSampler;
	};

	/** @} */
}
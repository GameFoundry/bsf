//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DPrerequisites.h"
#include "GpuBackend/B3DGpuCommandCapture.h"

// TODO - Surround if an #if so this can be included from non-Vulkan render backends (all other Vulkan specific code too)
#include <vulkan/vulkan.h>

namespace b3d
{
	/**
	 * Captures GPU commands for the RenderDoc tool. Captures are only made when the application runs under RenderDoc, which injects
	 * its library before the graphics API starts. Otherwise Start() and Stop() do nothing.
	 */
	class RenderDocCommandCapture : public GpuCommandCapture
	{
	public:
		explicit RenderDocCommandCapture(VkInstance vulkanInstance);

		void Start() override;
		void Stop() override;

	private:
		const VkInstance mVulkanInstance;
		void* mRenderDocAPIPointers = nullptr;
		bool mIsCaptureInProgress = false;

	};
} // namespace b3d

//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "Testing/B3DTestSuite.h"

namespace b3d
{
	/** End-to-end tests for Vulkan layout transitions, queue sharing and frame fences. */
	class VulkanBarrierTestSuite : public TestSuite
	{
	public:
		VulkanBarrierTestSuite();

	private:
		/**
		 * Checks that buffers, sampleable textures and concurrent-read textures use concurrent queue-family sharing when there is
		 * more than one family, and that other textures stay exclusive.
		 */
		void TestQueueSharing();

		/** Clears, loads and reads back color and depth/stencil attachments across render-pass layout transitions. */
		void TestRenderPassAttachmentTransitions();

		/** Presents acquired, rendered and read-back images, then recreates the swapchain. Uses a real swapchain when running without --headless. */
		void TestSwapChainTransitions();

		/** Resolves a multisampled texture through the Vulkan transfer-native resolve path. */
		void TestMultisampleResolve();

		/** Two dispatches reuse one storage-image binding and consume each other's writes. */
		void TestRepeatedStorageImageDispatch();

		/** Checks that a frame boundary arms frame-start waits on every queue, and that each queue's next submission alone consumes them. */
		void TestFrameFence();
	};
} // namespace b3d

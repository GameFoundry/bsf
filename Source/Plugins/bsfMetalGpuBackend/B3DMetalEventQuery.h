//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DMetalPrerequisites.h"
#include "GpuBackend/B3DEventQuery.h"
#include <atomic>

namespace b3d
{
	namespace render
	{
		class MetalGpuDevice;

		/** @addtogroup MetalGpuBackend
		 *  @{
		 */

		/**
		 * Metal event query.
		 *
		 * Uses an @c MTLSharedEvent to signal a monotonically-increasing value from the GPU, which the
		 * CPU can poll via @c signaledValue. Begin() makes the queue signal it once the command buffer it
		 * was recorded on finishes executing, as Metal 4 command buffers cannot signal events themselves.
		 */
		class MetalEventQuery final : public EventQuery
		{
		public:
			MetalEventQuery(MetalGpuDevice& gpuDevice);
			~MetalEventQuery() override;

			void Begin(GpuCommandBuffer& commandBuffer) override;
			bool IsReady() const override;

		private:
			id<MTLSharedEvent> mEvent = nil;
			std::atomic<u64> mExpectedValue{ 0 };
		};

		/** @} */
	} // namespace render
} // namespace b3d

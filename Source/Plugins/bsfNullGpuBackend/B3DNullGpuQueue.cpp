//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DNullGpuQueue.h"
#include "B3DNullGpuDevice.h"
#include "GpuBackend/B3DGpuCommandBuffer.h"

namespace b3d
{
	namespace render
	{
		NullGpuQueue::NullGpuQueue(GpuDevice& device, GpuQueueType type, u32 index)
			: GpuQueue(device, type, index)
		{ }

		void NullGpuQueue::SubmitCommandBuffer(const GpuSubmissionInformation& information)
		{
			// Nothing executes on the null device, so submitted work completes immediately
			GpuCommandBuffer* const commandBuffer = information.CommandBuffer.get();
			if(commandBuffer == nullptr)
				return;

			commandBuffer->OnDidComplete();
			commandBuffer->OnDidComplete.Clear();
			commandBuffer->OnDestroyed.Clear();
		}
	} // namespace render
} // namespace b3d

//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DMetalEventQuery.h"
#include "B3DMetalGpuDevice.h"
#include "B3DMetalGpuCommandBuffer.h"
#include "Profiling/B3DRenderStats.h"

namespace b3d
{
	namespace render
	{
		MetalEventQuery::MetalEventQuery(MetalGpuDevice& gpuDevice)
		{
			B3D_INCREMENT_RENDER_STATISTIC_CATEGORY(ResCreated, RenderStatObject_Query);
			id<MTLDevice> device = gpuDevice.GetMetalDevice();
			if (device != nil)
				mEvent = [device newSharedEvent];
		}

		MetalEventQuery::~MetalEventQuery()
		{
			B3D_INCREMENT_RENDER_STATISTIC_CATEGORY(ResDestroyed, RenderStatObject_Query);
		}

		void MetalEventQuery::Begin(GpuCommandBuffer& commandBuffer)
		{
			if (mEvent == nil)
				return;

			auto& metalCB = static_cast<MetalGpuCommandBuffer&>(commandBuffer);

			const u64 nextValue = mExpectedValue.load(std::memory_order_relaxed) + 1;
			if (metalCB.QueueEventSignal(mEvent, nextValue))
				mExpectedValue.store(nextValue, std::memory_order_release);
		}

		bool MetalEventQuery::IsReady() const
		{
			if (mEvent == nil)
				return false;

			const u64 target = mExpectedValue.load(std::memory_order_acquire);
			return target != 0 && mEvent.signaledValue >= target;
		}
	} // namespace render
} // namespace b3d

//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DGpuQueue.h"
#include "B3DRenderWindow.h"
#include "Profiling/B3DRenderStats.h"

using namespace b3d;

const GpuQueueMask GpuQueueMask::kNone = GpuQueueMask(0);
const GpuQueueMask GpuQueueMask::kAll = GpuQueueMask(~0u);

GpuQueue::GpuQueue(GpuDevice& gpuDevice, GpuQueueType type, u32 index)
	:mGpuDevice(gpuDevice), mType(type), mIndex(index)
{
}

void GpuQueue::PresentRenderWindow(const TShared<render::RenderWindow>& renderWindow, GpuQueueMask syncMask)
{
	if(renderWindow == nullptr)
		return;

	render::IRenderWindowSurface* const surface = renderWindow->GetRenderWindowSurface().get();
	if(surface == nullptr)
		return;

	renderWindow->NotifySwapBuffersRequested();

	// Captures read the image before the present engine takes it over
	renderWindow->ResolveCaptures(*this, syncMask);

	surface->SwapBuffers(*this, syncMask);

	B3D_INCREMENT_RENDER_STATISTIC(NumPresents);
}

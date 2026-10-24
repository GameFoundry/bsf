//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DVulkanResource.h"
#include "B3DVulkanGpuCommandBuffer.h"
#include "CoreObject/B3DRenderThread.h"
#include "GpuBackend/B3DGpuSwapChain.h"

using namespace b3d;
using namespace b3d::render;

template<class TBase>
VulkanGpuDevice& TVulkanResource<TBase>::GetDevice() const
{
	return mOwner->GetDevice();
}

namespace b3d::render
{
	template class TVulkanResource<IGpuResource>;
	template class TVulkanResource<IGpuBufferResource>;
	template class TVulkanResource<IGpuImageResource>;
	template class TVulkanResource<GpuSwapChain>;
}

VulkanResourceManager::VulkanResourceManager(VulkanGpuDevice& device)
	: GpuResourceManager(device)
{}

VulkanGpuDevice& VulkanResourceManager::GetDevice() const
{
	return static_cast<VulkanGpuDevice&>(mDevice);
}

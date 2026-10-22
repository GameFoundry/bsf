//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DMetalResidencySet.h"
#include "Debug/B3DLog.h"

namespace b3d::render
{
	MetalResidencySet::MetalResidencySet(id<MTLDevice> device)
	{
		MTLResidencySetDescriptor* descriptor = [[MTLResidencySetDescriptor alloc] init];
		descriptor.label = @"Banshee device residency set";
		descriptor.initialCapacity = 256;

		NSError* error = nil;
		mResidencySet = [device newResidencySetWithDescriptor:descriptor error:&error];
		if (mResidencySet == nil)
		{
			B3D_LOG(Fatal, LogRenderBackend, "Failed to create the Metal residency set: {0}",
				error ? String([[error localizedDescription] UTF8String]) : String("No error details were provided."));
		}
	}

	void MetalResidencySet::Add(id<MTLAllocation> allocation)
	{
		if (allocation == nil)
			return;

		Lock lock(mMutex);
		[mResidencySet addAllocation:allocation];
		mHasPendingChanges = true;
	}

	void MetalResidencySet::Remove(id<MTLAllocation> allocation)
	{
		if (allocation == nil)
			return;

		Lock lock(mMutex);
		[mResidencySet removeAllocation:allocation];
		mHasPendingChanges = true;
	}

	void MetalResidencySet::Commit(const Function<void()>& fnCommitCommandBuffers)
	{
		Lock lock(mMutex);
		if (mHasPendingChanges)
		{
			[mResidencySet commit];
			mHasPendingChanges = false;
		}

		fnCommitCommandBuffers();
	}

#if B3D_BUILD_TYPE_DEVELOPMENT
	bool MetalResidencySet::IsResident(id<MTLResource> resource) const
	{
		if (resource == nil)
			return true;

		// Views share the allocation of the resource they were created from
		id<MTLResource> allocation = resource;
		while ([allocation conformsToProtocol:@protocol(MTLTexture)])
		{
			id<MTLTexture> texture = (id<MTLTexture>)allocation;
			if (texture.parentTexture != nil)
				allocation = texture.parentTexture;
			else if (texture.buffer != nil)
				allocation = texture.buffer;
			else
				break;
		}

		Lock lock(mMutex);
		if (allocation.heap != nil)
			return [mResidencySet containsAllocation:allocation.heap];

		return [mResidencySet containsAllocation:allocation];
	}
#endif
} // namespace b3d::render

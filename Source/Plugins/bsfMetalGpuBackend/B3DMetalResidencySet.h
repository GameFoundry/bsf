//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DMetalPrerequisites.h"
#include "Threading/B3DThreading.h"

namespace b3d::render
{
	/** @addtogroup MetalGpuBackend
	 *  @{
	 */

	/**
	 * Set of allocations (heaps, buffers and textures) kept resident for every command buffer committed to the queues the set
	 * is attached to. Metal 4 command buffers can only access resident memory, and accessing memory that is not resident
	 * fails silently, so every allocation the GPU may access must be added before the command buffers that access it are
	 * committed. Resources placed in a heap are covered by adding the heap.
	 *
	 * @note	Thread safe.
	 */
	class MetalResidencySet
	{
	public:
		MetalResidencySet(id<MTLDevice> device);

		/** Makes @p allocation resident for command buffers committed after the next Commit(). */
		void Add(id<MTLAllocation> allocation);

		/**
		 * Ends the residency of @p allocation on the next Commit(). The GPU must have finished every command buffer that
		 * accesses it.
		 */
		void Remove(id<MTLAllocation> allocation);

		/**
		 * Applies the changes made since the last commit, then calls @p fnCommitCommandBuffers, which commits the command
		 * buffers that depend on them. No other thread can change the set until it returns, as the queue reads the set
		 * while committing.
		 */
		void Commit(const Function<void()>& fnCommitCommandBuffers);

#if B3D_BUILD_TYPE_DEVELOPMENT
		/** Returns true if @p resource is resident, either on its own or through the heap it is placed in. */
		bool IsResident(id<MTLResource> resource) const;
#endif

		/** Returns the native residency set. */
		id<MTLResidencySet> GetMetalResidencySet() const { return mResidencySet; }

	private:
		id<MTLResidencySet> mResidencySet = nil;
		bool mHasPendingChanges = false;
		mutable Mutex mMutex;
	};

	/** @} */
} // namespace b3d::render

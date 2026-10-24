//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DMetalPrerequisites.h"
#include "GpuBackend/B3DGpuBarriers.h"

namespace b3d::render
{
	/** @addtogroup MetalGpuBackend
	 *  @{
	 */

	/**
	 * Dependency between two sets of Metal stages, as encoded by the Metal 4 stage barriers. Work in the source stages
	 * completes, and with a visibility other than MTL4VisibilityOptionNone its writes become visible, before work in the
	 * destination stages starts.
	 */
	struct MetalStageBarrier
	{
		MetalStageBarrier() = default;

		/**
		 * Converts an engine barrier scope. Writes in the source scope make the barrier flush caches, and @p flags with
		 * GpuBarrierFlag::AliasAcquire make it flush the caches of aliased memory.
		 */
		MetalStageBarrier(const GpuBarrierScope& scope, GpuBarrierFlags flags = GpuBarrierFlag::None);

		/** Adds the stages and visibility of @p other to this barrier. */
		void Merge(const MetalStageBarrier& other);

		/** Returns true if the barrier orders nothing, because work on either side doesn't run on the GPU. */
		bool IsEmpty() const { return SourceStages == 0 || DestinationStages == 0; }

		MTLStages SourceStages = 0;
		MTLStages DestinationStages = 0;
		MTL4VisibilityOptions Visibility = MTL4VisibilityOptionNone;
	};

	/** @} */
} // namespace b3d::render

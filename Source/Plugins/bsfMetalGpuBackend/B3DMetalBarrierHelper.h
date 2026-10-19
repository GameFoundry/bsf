//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "B3DMetalPrerequisites.h"
#include "GpuBackend/B3DGpuBarrierHelper.h"

namespace b3d::render
{
	class MetalResourceTracker;

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

	/**
	 * Helper class for building Metal 4 stage barriers from the barriers the resource tracker resolves.
	 *
	 * Metal 4 barriers are stage-to-stage rather than per-resource, so the helper only accumulates the union of the
	 * resolved barriers, and leaves encoding to MetalGpuCommandBuffer, which knows what the open encoder contains.
	 * Image layouts are pure bookkeeping on Metal, the base's layout tracking still runs so the tracker state stays
	 * coherent.
	 *
	 * Typical usage (from MetalGpuCommandBuffer):
	 * @code
	 * mResourceTracker.TrackBufferAccess(buffer, stages, access, mBarrierHelper);
	 * EncodeBarrier(mBarrierHelper.Execute()); // before recording the dependent command
	 * @endcode
	 */
	class MetalBarrierHelper : public TGpuBarrierHelper<MetalBarrierHelper, MetalResourceTracker>
	{
	public:
		/**
		 * Constructs a barrier helper associated with the provided resource tracker.
		 *
		 * @param resourceTracker	Object responsible for tracking all resource usages on a command buffer. Used for
		 *							determining current object state, and notified with new state when barriers and
		 *							layout transitions are executed.
		 */
		MetalBarrierHelper(MetalResourceTracker* resourceTracker);

		/**
		 * Returns the barrier accumulated since the last Execute() or Clear(), then runs the post-barrier tracker updates
		 * (ApplyPostBarrierTracking, CommitPendingAccesses) and clears the accumulated state. Always call this after a
		 * batch of Track*Usage / Track*Barrier calls and before recording the dependent commands, even when HasBarriers()
		 * is false, so deferred hazard registrations commit at the right point. The caller must encode the returned
		 * barrier.
		 */
		MetalStageBarrier Execute();

		/**
		 * Clears all accumulated barriers without executing them. Useful if you need to reset the
		 * helper state without issuing barriers.
		 */
		void Clear();

		/** Returns true if there are any barriers accumulated and ready to execute. */
		bool HasBarriers() const { return !mBarrier.IsEmpty(); }

		/**
		 * Returns true if an accumulated barrier waits for fragment shader work that an open render encoder may have
		 * encoded, which no barrier within the encoder can wait for.
		 *
		 * @param	encoderHasFragmentWrites	True if fragment shaders of the open render encoder may write storage resources.
		 */
		bool WaitsForEncoderFragmentWork(bool encoderHasFragmentWrites) const;

		/**
		 * Returns true if a barrier from @p sourceStages, to work that accesses resources with @p destinationAccess, waits
		 * for fragment shader work that an open render encoder may have encoded.
		 *
		 * @param	sourceStages				Engine stages the barrier waits for.
		 * @param	destinationAccess			Accesses of the work that waits.
		 * @param	encoderHasFragmentWrites	True if fragment shaders of the open render encoder may write storage resources.
		 */
		static bool WaitsForFragmentShaderWork(GpuStageFlags sourceStages, GpuAccessFlags destinationAccess, bool encoderHasFragmentWrites);

	private:
		friend class TGpuBarrierHelper<MetalBarrierHelper, MetalResourceTracker>;

		/** Accumulates a resolved buffer barrier. */
		void RecordNativeBufferBarrier(IGpuBufferResource* buffer, const GpuBarrierScope& barrier, GpuBarrierFlags barrierFlags);

		/**
		 * Accumulates a resolved image barrier. Metal performs no native layout transitions, so unlike the Vulkan hook
		 * @p oldLayout is taken by value and never reconciled, the base's layout bookkeeping proceeds with the tracked
		 * value unchanged.
		 */
		void RecordNativeImageBarrier(IGpuImageResource* image, const GpuTextureSubresourceRange& subresourceRange, const GpuBarrierScope& barrier, GpuImageLayout oldLayout, GpuImageLayout newLayout, GpuBarrierFlags barrierFlags);

		MetalStageBarrier mBarrier;

		/** Engine stages and accesses of the accumulated barriers, which tell apart work the Metal stages merge. */
		GpuStageFlags mSourceStages = GpuStageFlag::None;
		GpuAccessFlags mDestinationAccess = GpuAccessFlag::None;
	};

	extern template class TGpuBarrierHelper<MetalBarrierHelper, MetalResourceTracker>;

	/** @} */
} // namespace b3d::render

//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DMetalBarrierHelper.h"
#include "B3DMetalResourceTracker.h"
#include "B3DMetalUtility.h"
#include "GpuBackend/B3DGpuBackendUtility.h"

// Generic barrier-helper method definitions, followed by the explicit instantiation for the Metal
// barrier helper. Included here (after the complete MetalBarrierHelper and MetalResourceTracker
// types are available) so the single instantiation lives in this translation unit. The header
// carries a matching `extern template` to suppress implicit instantiation elsewhere.
#include "GpuBackend/B3DGpuBarrierHelper.inl"

template class b3d::render::TGpuBarrierHelper<b3d::render::MetalBarrierHelper, b3d::render::MetalResourceTracker>;

namespace b3d::render
{
	MetalStageBarrier::MetalStageBarrier(const GpuBarrierScope& scope, GpuBarrierFlags flags)
		: SourceStages(MetalUtility::GetStages(scope.SourceStages)), DestinationStages(MetalUtility::GetStages(scope.DestinationStages))
	{
		if (scope.SourceAccess.IsSet(GpuAccessFlag::Write))
			Visibility |= MTL4VisibilityOptionDevice;

		if (flags.IsSet(GpuBarrierFlag::AliasAcquire))
			Visibility |= MTL4VisibilityOptionResourceAlias;
	}

	void MetalStageBarrier::Merge(const MetalStageBarrier& other)
	{
		if (other.IsEmpty())
			return;

		SourceStages |= other.SourceStages;
		DestinationStages |= other.DestinationStages;
		Visibility |= other.Visibility;
	}

	MetalBarrierHelper::MetalBarrierHelper(MetalResourceTracker* resourceTracker)
		: TGpuBarrierHelper<MetalBarrierHelper, MetalResourceTracker>(resourceTracker)
	{ }

	void MetalBarrierHelper::RecordNativeBufferBarrier(IGpuBufferResource* buffer, const GpuBarrierScope& barrier, GpuBarrierFlags barrierFlags)
	{
		(void)buffer;

		mBarrier.Merge(MetalStageBarrier(barrier, barrierFlags));
		mSourceStages |= barrier.SourceStages;
		mDestinationAccess |= barrier.DestinationAccess;
	}

	void MetalBarrierHelper::RecordNativeImageBarrier(IGpuImageResource* image, const GpuTextureSubresourceRange& subresourceRange, const GpuBarrierScope& barrier, GpuImageLayout oldLayout, GpuImageLayout newLayout, GpuBarrierFlags barrierFlags)
	{
		(void)image;
		(void)subresourceRange;
		(void)oldLayout;
		(void)newLayout;

		mBarrier.Merge(MetalStageBarrier(barrier, barrierFlags));
		mSourceStages |= barrier.SourceStages;
		mDestinationAccess |= barrier.DestinationAccess;
	}

	MetalStageBarrier MetalBarrierHelper::Execute()
	{
		const MetalStageBarrier barrier = mBarrier;

		// Post-barrier bookkeeping runs unconditionally: Track*Usage defers hazard registrations even
		// when no native barrier was queued, and they must commit at this point (see the core
		// CommitPendingAccesses contract).
		ApplyPostBarrierTracking();
		mResourceTracker->CommitPendingAccesses();
		Clear();

		return barrier;
	}

	void MetalBarrierHelper::Clear()
	{
		mBarrier = MetalStageBarrier();
		mSourceStages = GpuStageFlag::None;
		mDestinationAccess = GpuAccessFlag::None;

		TGpuBarrierHelper<MetalBarrierHelper, MetalResourceTracker>::Clear();
	}

	bool MetalBarrierHelper::WaitsForEncoderFragmentWork(bool encoderHasFragmentWrites) const
	{
		return HasBarriers() && WaitsForFragmentShaderWork(mSourceStages, mDestinationAccess, encoderHasFragmentWrites);
	}

	bool MetalBarrierHelper::WaitsForFragmentShaderWork(GpuStageFlags sourceStages, GpuAccessFlags destinationAccess, bool encoderHasFragmentWrites)
	{
		// Attachments of other render passes were accessed by earlier encoders, as a render pass cannot access its own
		// attachments from shaders. Fragment shader reads within the encoder only matter if something writes after them.
		const GpuStageFlags fragmentShaderStages = GpuStageFlag::FragmentShaderNonUniform | GpuStageFlag::FragmentShaderUniform;
		if (!sourceStages.IsSetAny(fragmentShaderStages))
			return false;

		return encoderHasFragmentWrites || destinationAccess.IsSet(GpuAccessFlag::Write);
	}
} // namespace b3d::render

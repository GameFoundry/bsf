//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DMetalStageBarrier.h"
#include "B3DMetalUtility.h"

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
} // namespace b3d::render

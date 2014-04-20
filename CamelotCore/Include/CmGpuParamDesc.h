#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmGpuProgramParams.h" // TODO - Only here because I need some type definitions (GpuConstantType) - Remove later

namespace BansheeEngine
{
	struct GpuParamDataDesc
	{
		String name;
		UINT32 elementSize; // Multiple of 4 bytes
		UINT32 arraySize;
		UINT32 arrayElementStride; // Multiple of 4 bytes
		GpuParamDataType type;

		UINT32 paramBlockSlot;
		UINT32 gpuMemOffset;
		UINT32 cpuMemOffset;
	};

	struct GpuParamObjectDesc
	{
		String name;
		GpuParamObjectType type;

		UINT32 slot;
	};

	struct GpuParamBlockDesc
	{
		String name;
		UINT32 slot;
		UINT32 blockSize;
		bool isShareable;
	};

	struct GpuParamDesc
	{
		Map<String, GpuParamBlockDesc>::type paramBlocks;
		Map<String, GpuParamDataDesc>::type params;

		Map<String, GpuParamObjectDesc>::type samplers;
		Map<String, GpuParamObjectDesc>::type textures;
		Map<String, GpuParamObjectDesc>::type buffers;
	};
}
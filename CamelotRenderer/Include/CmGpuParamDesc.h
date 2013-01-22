#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmGpuProgramParams.h" // TODO - Only here because I need some type definitions (GpuConstantType) - Remove later

namespace CamelotEngine
{
	struct GpuParamDataDesc
	{
		String name;
		UINT32 elementSize; // Multiple of 4 bytes
		UINT32 arraySize;
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
		map<String, GpuParamBlockDesc>::type paramBlocks;
		map<String, GpuParamDataDesc>::type params;

		map<String, GpuParamObjectDesc>::type samplers;
		map<String, GpuParamObjectDesc>::type textures;
		map<String, GpuParamObjectDesc>::type buffers;
	};
}
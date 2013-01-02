#pragma once

#include "CmPrerequisites.h"
#include "CmGpuProgramParams.h" // TODO - Only here because I need some type definitions (GpuConstantType) - Remove later

namespace CamelotEngine
{
	enum GpuMemberType
	{
		GMT_FLOAT1 = 1,
		GMT_FLOAT2 = 2,
		GMT_FLOAT3 = 3,
		GMT_FLOAT4 = 4,
		GMT_MATRIX_2X2 = 11,
		GMT_MATRIX_2X3 = 12,
		GMT_MATRIX_2X4 = 13,
		GMT_MATRIX_3X2 = 14,
		GMT_MATRIX_3X3 = 15,
		GMT_MATRIX_3X4 = 16,
		GMT_MATRIX_4X2 = 17,
		GMT_MATRIX_4X3 = 18,
		GMT_MATRIX_4X4 = 19,
		GMT_INT1 = 20,
		GMT_INT2 = 21,
		GMT_INT3 = 22,
		GMT_INT4 = 23,
		GMT_BOOL = 24,
		GMT_UNKNOWN = 99
	};

	enum GpuSpecialType
	{
		GST_SAMPLER1D = 1,
		GST_SAMPLER2D = 2,
		GST_SAMPLER3D = 3,
		GST_SAMPLERCUBE = 4,
		GST_TEXTURE1D = 11,
		GST_TEXTURE2D = 12,
		GST_TEXTURE3D = 13,
		GST_TEXTURECUBE = 14
	};

	struct GpuParamMemberDesc
	{
		String name;
		UINT32 elementSize; // Multiple of 4 bytes
		UINT32 arraySize;
		GpuMemberType type;

		UINT32 paramBlockSlot;
		UINT32 gpuMemOffset;
		UINT32 cpuMemOffset;
	};

	struct GpuParamSpecialDesc
	{
		String name;
		GpuSpecialType type;

		UINT32 slot;
	};

	struct GpuParamBlockDesc
	{
		String name;
		UINT32 slot;
	};

	struct GpuParamDesc
	{
		map<String, GpuParamBlockDesc>::type paramBlocks;
		map<String, GpuParamMemberDesc>::type params;

		map<String, GpuParamSpecialDesc>::type samplers;
		map<String, GpuParamSpecialDesc>::type textures;
		map<String, GpuParamSpecialDesc>::type buffers;
	};
}
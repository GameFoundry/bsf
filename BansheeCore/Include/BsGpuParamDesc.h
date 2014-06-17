#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Describes a single GPU program data (e.g. int, float, Vector2) parameter.
	 */
	struct GpuParamDataDesc
	{
		String name;
		UINT32 elementSize; /**< In multiples of 4 bytes. */
		UINT32 arraySize;
		UINT32 arrayElementStride; /**< In multiples of 4 bytes. */
		GpuParamDataType type;

		UINT32 paramBlockSlot;
		UINT32 gpuMemOffset;
		UINT32 cpuMemOffset;
	};

	/**
	 * @brief	Describes a single GPU program object (e.g. texture, sampler state) parameter.
	 */
	struct GpuParamObjectDesc
	{
		String name;
		GpuParamObjectType type;

		UINT32 slot;
	};

	/**
	 * @brief	Describes a GPU program parameter block (collection of GPU program data parameters).
	 */
	struct GpuParamBlockDesc
	{
		String name;
		UINT32 slot;
		UINT32 blockSize; /**< In multiples of 4 bytes. */
		bool isShareable;
	};

	/**
	 * @brief	Contains all parameter information for a GPU program, including data and object parameters,
	 *			plus parameter blocks.
	 */
	struct GpuParamDesc
	{
		Map<String, GpuParamBlockDesc> paramBlocks;
		Map<String, GpuParamDataDesc> params;

		Map<String, GpuParamObjectDesc> samplers;
		Map<String, GpuParamObjectDesc> textures;
		Map<String, GpuParamObjectDesc> buffers;
	};
}
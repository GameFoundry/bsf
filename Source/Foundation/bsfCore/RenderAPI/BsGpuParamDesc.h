//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"

namespace bs
{
	/** @addtogroup RenderAPI
	 *  @{
	 */

	/** Describes a single GPU program data (for example int, float, Vector2) parameter. */
	struct GpuParamDataDesc
	{
		String name;
		UINT32 elementSize; /**< In multiples of 4 bytes. */
		UINT32 arraySize;
		UINT32 arrayElementStride; /**< In multiples of 4 bytes. */
		GpuParamDataType type;

		UINT32 paramBlockSlot;
		UINT32 paramBlockSet;
		UINT32 gpuMemOffset; /**< In multiples of 4 bytes, or index for parameters not in a buffer. */
		UINT32 cpuMemOffset; /**< In multiples of 4 bytes. */
	};

	/**	Describes a single GPU program object (for example texture, sampler state) parameter. */
	struct GpuParamObjectDesc
	{
		String name;
		GpuParamObjectType type;

		/** Slot within a set. Uniquely identifies bind location in the GPU pipeline, together with the set. */
		UINT32 slot;

		/** Uniquely identifies the bind location in the GPU pipeline, together with the slot. */
		UINT32 set;

		/** Underlying type of individual elements in the buffer or texture. */
		GpuBufferFormat elementType = BF_UNKNOWN;
	};

	/**	Describes a GPU program parameter block (collection of GPU program data parameters). */
	struct GpuParamBlockDesc
	{
		String name;
		UINT32 slot; /** Slot within a set. Uniquely identifies bind location in the GPU pipeline, together with the set. */
		UINT32 set; /** Uniquely identifies the bind location in the GPU pipeline, together with the slot. */
		UINT32 blockSize; /**< In multiples of 4 bytes. */
		bool isShareable; /** True for blocks that can be shared between different GPU pipeline stages. */
	};

	/** Contains all parameter information for a GPU program, including data and object parameters, plus parameter blocks. */
	struct BS_CORE_EXPORT GpuParamDesc : IReflectable
	{
		Map<String, GpuParamBlockDesc> paramBlocks;
		Map<String, GpuParamDataDesc> params;

		Map<String, GpuParamObjectDesc> samplers;
		Map<String, GpuParamObjectDesc> textures;
		Map<String, GpuParamObjectDesc> loadStoreTextures;
		Map<String, GpuParamObjectDesc> buffers;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuParamDescRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}

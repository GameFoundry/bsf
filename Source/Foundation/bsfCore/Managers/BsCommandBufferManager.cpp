//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Managers/BsCommandBufferManager.h"

namespace bs { namespace ct
{
	SPtr<CommandBuffer> CommandBufferManager::create(GpuQueueType type, UINT32 deviceIdx, UINT32 queueIdx,
		bool secondary)
	{
		assert(deviceIdx < BS_MAX_DEVICES);

		return createInternal(type, deviceIdx, queueIdx, secondary);
	}
}}

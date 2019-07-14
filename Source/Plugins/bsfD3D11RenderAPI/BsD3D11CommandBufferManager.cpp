//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsD3D11CommandBufferManager.h"
#include "BsD3D11CommandBuffer.h"

namespace bs { namespace ct
{
	SPtr<CommandBuffer> D3D11CommandBufferManager::createInternal(GpuQueueType type, UINT32 deviceIdx,
		UINT32 queueIdx, bool secondary)
	{
		CommandBuffer* buffer = new (bs_alloc<D3D11CommandBuffer>()) D3D11CommandBuffer(type, deviceIdx, queueIdx, secondary);
		return bs_shared_ptr(buffer);
	}
}}

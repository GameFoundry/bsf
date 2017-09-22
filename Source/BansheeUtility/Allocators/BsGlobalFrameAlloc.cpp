//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Prerequisites/BsPrerequisitesUtil.h"
#include "Allocators/BsGlobalFrameAlloc.h"
#include "Allocators/BsFrameAlloc.h"

namespace bs
{
	BS_THREADLOCAL FrameAlloc* _GlobalFrameAlloc = nullptr;

	BS_UTILITY_EXPORT FrameAlloc& gFrameAlloc()
	{
		if (_GlobalFrameAlloc == nullptr)
		{
			// Note: This will leak memory but since it should exist throughout the entirety 
			// of runtime it should only leak on shutdown when the OS will free it anyway.
			_GlobalFrameAlloc = new FrameAlloc();
		}

		return *_GlobalFrameAlloc;
	}

	BS_UTILITY_EXPORT UINT8* bs_frame_alloc(UINT32 numBytes)
	{
		return gFrameAlloc().alloc(numBytes);
	}

	BS_UTILITY_EXPORT UINT8* bs_frame_alloc_aligned(UINT32 count, UINT32 align)
	{
		return gFrameAlloc().allocAligned(count, align);
	}

	BS_UTILITY_EXPORT void bs_frame_free(void* data)
	{
		gFrameAlloc().dealloc((UINT8*)data);
	}

	BS_UTILITY_EXPORT void bs_frame_free_aligned(void* data)
	{
		gFrameAlloc().dealloc((UINT8*)data);
	}

	BS_UTILITY_EXPORT void bs_frame_mark()
	{
		gFrameAlloc().markFrame();
	}

	BS_UTILITY_EXPORT void bs_frame_clear()
	{
		gFrameAlloc().clear();
	}
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	/** @addtogroup Internal-Utility
	 *  @{
	 */

	/** @addtogroup Memory-Internal
	 *  @{
	 */

	/**
	 * Free allocator with no limitations, using traditional malloc/free under the hood. */
	class FreeAlloc
	{
	public:
		/** Allocates memory. */
		UINT8* alloc(UINT32 amount)
		{
			return (UINT8*)malloc(amount);
		}

		/** Deallocates a previously allocated piece of memory. */
		void free(void* data)
		{
			::free(data);
		}

		/** Unused */
		void clear()
		{
			// Do nothing
		}
	};

	/** @} */
	/** @} */
}

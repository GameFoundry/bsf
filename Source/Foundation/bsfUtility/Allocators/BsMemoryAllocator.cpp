//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Prerequisites/BsPrerequisitesUtil.h"

namespace bs
{
	UINT64 BS_THREADLOCAL MemoryCounter::Allocs = 0;
	UINT64 BS_THREADLOCAL MemoryCounter::Frees = 0;
}

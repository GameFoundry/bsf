#include "CmPrerequisitesUtil.h"
#include "CmMemStack.h"

namespace CamelotFramework
{
	MemStackInternal<1024 * 1024> MemStack::mStacks[256];

#if CM_DEBUG_MODE
	CM_THREAD_ID_TYPE MemStack::mThreadIds[256];
#endif

	void MemStack::setupHeap(UINT8 heapId)
	{
		assert(heapId < 256);

		mStacks[heapId] = MemStackInternal<1024 * 1024>();

#if CM_DEBUG_MODE
		mThreadIds[heapId] = CM_THREAD_CURRENT_ID;
#endif
	}

	UINT8* MemStack::alloc(UINT32 numBytes, UINT32 heapId)
	{
#if CM_DEBUG_MODE
		assert(mThreadIds[heapId] == CM_THREAD_CURRENT_ID && "Accessing a heap from an invalid thread.");
#endif

		return mStacks[heapId].alloc(numBytes);
	}

	void MemStack::deallocLast(UINT8* data, UINT32 heapId)
	{
#if CM_DEBUG_MODE
		assert(mThreadIds[heapId] == CM_THREAD_CURRENT_ID && "Accessing a heap from an invalid thread.");
#endif

		mStacks[heapId].dealloc(data);
	}

	UINT8* stackAlloc(UINT32 numBytes, UINT32 heapId)
	{
		return MemStack::alloc(numBytes, heapId);
	}

	void stackDeallocLast(void* data, UINT32 heapId)
	{
		return MemStack::deallocLast((UINT8*)data, heapId);
	}
}
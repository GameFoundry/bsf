#include "CmPrerequisitesUtil.h"
#include "CmMemStack.h"

namespace CamelotFramework
{
	MemStackInternal<1024 * 1024>* MemStack::ThreadMemStack = nullptr;

	void MemStack::beginThread()
	{
		if(ThreadMemStack != nullptr)
			endThread();

		ThreadMemStack = cm_new<MemStackInternal<1024 * 1024>>();
	}

	void MemStack::endThread()
	{
		if(ThreadMemStack != nullptr)
		{
			cm_delete(ThreadMemStack);
			ThreadMemStack = nullptr;
		}
	}

	UINT8* MemStack::alloc(UINT32 numBytes)
	{
		assert(ThreadMemStack != nullptr && "Stack allocation failed. Did you call beginThread?");

		return ThreadMemStack->alloc(numBytes);
	}

	void MemStack::deallocLast(UINT8* data)
	{
		assert(ThreadMemStack != nullptr && "Stack deallocation failed. Did you call beginThread?");

		ThreadMemStack->dealloc(data);
	}

	UINT8* stackAlloc(UINT32 numBytes)
	{
		return MemStack::alloc(numBytes);
	}

	void stackDeallocLast(void* data)
	{
		return MemStack::deallocLast((UINT8*)data);
	}
}
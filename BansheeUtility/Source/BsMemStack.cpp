#include "BsPrerequisitesUtil.h"
#include "BsMemStack.h"

namespace BansheeEngine
{
	MemStackInternal<1024 * 1024>* MemStack::ThreadMemStack = nullptr;

	void MemStack::beginThread()
	{
		if(ThreadMemStack != nullptr)
			endThread();

		ThreadMemStack = bs_new<MemStackInternal<1024 * 1024>>();
	}

	void MemStack::endThread()
	{
		if(ThreadMemStack != nullptr)
		{
			bs_delete(ThreadMemStack);
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

	void* stackAlloc(UINT32 numBytes)
	{
		return (void*)MemStack::alloc(numBytes);
	}

	void stackDeallocLast(void* data)
	{
		return MemStack::deallocLast((UINT8*)data);
	}
}
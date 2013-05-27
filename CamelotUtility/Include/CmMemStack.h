#pragma once

#include <stack>
#include <assert.h>
#include "CmThreadDefines.h"

namespace CamelotFramework
{
	template <int BlockCapacity = 1024 * 1024>
	class MemStackInternal
	{
	private:
		class MemBlock
		{
		public:
			MemBlock(UINT32 size)
				:mData(nullptr), mFreePtr(0), mSize(size)
			{
				mData = static_cast<UINT8*>(CM_NEW_BYTES(mSize, GenAlloc));
			}

			~MemBlock()
			{
				CM_DELETE_BYTES(mData, GenAlloc);
			}

			UINT8* alloc(UINT8 amount)
			{
				UINT8* freePtr = &mData[mFreePtr];
				mFreePtr += amount;

				return freePtr;
			}

			void dealloc(UINT8* data, UINT8 amount)
			{
				mFreePtr -= amount;
				assert((&mData[mFreePtr]) == data && "Out of order stack deallocation detected. Deallocations need to happen in order opposite of allocations.");
			}

			UINT8* mData;
			UINT32 mFreePtr;
			UINT32 mSize;
		};

	public:
		MemStackInternal()
		{ }

		~MemStackInternal()
		{
			assert(mBlocks.size() == 0 && "Not all blocks were released before shutting down the stack allocator.");

			while(!mBlocks.empty())
			{
				MemBlock* curPtr = mBlocks.top();
				mBlocks.pop();

				CM_DELETE(curPtr, MemBlock, GenAlloc);
			}
		}

		UINT8* alloc(UINT32 amount)
		{
			MemBlock* topBlock;
			if(mBlocks.size() == 0)
				topBlock = allocNewBlock(amount);
			else
				topBlock = mBlocks.top();

			mAllocSizes.push(amount);

			UINT32 freeMem = topBlock->mSize - topBlock->mFreePtr;
			if(amount <= freeMem)
				return topBlock->alloc(amount);

			MemBlock* newBlock = allocNewBlock(amount);
			return newBlock->alloc(amount);
		}

		void dealloc(UINT8* data)
		{
			assert(mAllocSizes.size() > 0 && "Out of order stack deallocation detected. Deallocations need to happen in order opposite of allocations.");

			UINT32 amount = mAllocSizes.top();
			mAllocSizes.pop();

			MemBlock* topBlock = mBlocks.top();
			topBlock->dealloc(data, amount);

			if(topBlock->mFreePtr == 0)
			{
				CM_DELETE(topBlock, MemBlock, GenAlloc);
				mBlocks.pop();
			}
		}

	private:
		std::stack<MemBlock*> mBlocks;
		std::stack<UINT32> mAllocSizes;

		MemBlock* allocNewBlock(UINT32 wantedSize)
		{
			UINT32 blockSize = BlockCapacity;
			if(wantedSize > blockSize)
				blockSize = wantedSize;

			MemBlock* newBlock = CM_NEW(MemBlock, GenAlloc) MemBlock(blockSize);
			mBlocks.push(newBlock);

			return newBlock;
		}
	};

	/**
	 * @brief	Fastest, but also most limiting type of allocator. All deallocations
	 * 			must happen in opposite order from allocations. 
	 * 			
	 * @note	It's mostly useful when you need to allocate something temporarily on the heap,
	 * 			usually something that gets allocated and freed within the same function.
	 * 			
	 *			Each allocation comes with a pretty hefty 4 byte memory overhead, so don't use it for small allocations. 
	 *			
	 *			Operations done on a single heap are thread safe. Multiple threads are not allowed to access a heap that wasn't
	 *			created for them.
	 *
	 * @tparam	BlockCapacity Minimum size of a block. Larger blocks mean less memory allocations, but also potentially
	 * 						  more wasted memory. If an allocation requests more bytes than BlockCapacity, first largest multiple is
	 * 						  used instead.
	 * @tparam	VectorAligned If true, all allocations will be aligned to 16bit boundaries.
	 */
	class CM_UTILITY_EXPORT MemStack
	{
	public:
		/**
		 * @brief	Sets up the heap you can later use with alloc/dealloc calls. It is most common to have one heap
		 * 			per thread.
		 *
		 * @param	heapId	Unique heap ID. Each heap can only be used from one thread, it cannot be shared.
		 * 					You cannot have more than 256 heaps.
		 */
		static void setupHeap(UINT8 heapId);

		static UINT8* alloc(UINT32 numBytes, UINT32 heapId);
		static void deallocLast(UINT8* data, UINT32 heapId);

	private:
		static MemStackInternal<1024 * 1024> mStacks[256];

#if CM_DEBUG_MODE
		static CM_THREAD_ID_TYPE mThreadIds[256];
#endif
	};

	CM_UTILITY_EXPORT inline UINT8* stackAlloc(UINT32 numBytes, UINT32 heapId);

	template<class T>
	T* stackAlloc(UINT32 heapId)
	{
		return (T*)MemStack::alloc(sizeof(T), heapId);
	}

	template<class T>
	T* stackAllocN(UINT32 count, UINT32 heapId)
	{
		return (T*)MemStack::alloc(sizeof(T) * count, heapId);
	}

	template<class T>
	T* stackConstructN(UINT32 count, UINT32 heapId)
	{
		T* data = stackAllocN<T>(count, heapId);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T;

		return data;
	}

	template<class T>
	void stackDestruct(T* data, UINT32 heapId)
	{
		data->~T();

		MemStack::deallocLast((UINT8*)data, heapId);
	}

	template<class T>
	void stackDestructN(T* data, UINT32 count, UINT32 heapId)
	{
		for(unsigned int i = 0; i < count; i++)
			data[i].~T();

		MemStack::deallocLast((UINT8*)data, heapId);
	}

	CM_UTILITY_EXPORT inline void stackDeallocLast(void* data, UINT32 heapId);
}
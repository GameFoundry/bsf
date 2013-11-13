#pragma once

#include <stack>
#include <assert.h>
#include "CmThreadDefines.h"

namespace CamelotFramework
{
	/**
	 * @brief	Memory stack.
	 *
	 *  @tparam	BlockCapacity Minimum size of a block. Larger blocks mean less memory allocations, but also potentially
	 * 			more wasted memory. If an allocation requests more bytes than BlockCapacity, first largest multiple is
	 * 			used instead.
	 */
	template <int BlockCapacity = 1024 * 1024>
	class MemStackInternal
	{
	private:
		class MemBlock
		{
		public:
			MemBlock(UINT32 size)
				:mData(nullptr), mFreePtr(0), mSize(size)
			{ }

			~MemBlock()
			{ }

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

				deallocBlock(curPtr);
			}
		}

		UINT8* alloc(UINT32 amount)
		{
			amount += sizeof(UINT32);

			MemBlock* topBlock;
			if(mBlocks.size() == 0)
				topBlock = allocBlock(amount);
			else
				topBlock = mBlocks.top();

			MemBlock* memBlock = nullptr;
			UINT32 freeMem = topBlock->mSize - topBlock->mFreePtr;
			if(amount <= freeMem)
				memBlock = topBlock;
			else
				memBlock = allocBlock(amount);

			UINT8* data = memBlock->alloc(amount);

			UINT32* storedSize = reinterpret_cast<UINT32*>(data);
			*storedSize = amount;

			return data + sizeof(UINT32);
		}

		void dealloc(UINT8* data)
		{
			data -= sizeof(UINT32);

			UINT32* storedSize = reinterpret_cast<UINT32*>(data);

			MemBlock* topBlock = mBlocks.top();
			topBlock->dealloc(data, *storedSize);

			if(topBlock->mFreePtr == 0)
			{
				deallocBlock(topBlock);
				mBlocks.pop();
			}
		}

	private:
		std::stack<MemBlock*> mBlocks;

		MemBlock* allocBlock(UINT32 wantedSize)
		{
			UINT32 blockSize = BlockCapacity;
			if(wantedSize > blockSize)
				blockSize = wantedSize;

			UINT8* data = (UINT8*)reinterpret_cast<UINT8*>(cm_alloc(blockSize + sizeof(MemBlock)));
			MemBlock* newBlock = new (data) MemBlock(blockSize);
			data += sizeof(MemBlock);
			newBlock->mData = data;

			mBlocks.push(newBlock);

			return newBlock;
		}

		void deallocBlock(MemBlock* block)
		{
			block->~MemBlock();
			cm_free(block);
		}
	};

	/**
	 * @brief	One of the fastest, but also very limiting type of allocator. All deallocations
	 * 			must happen in opposite order from allocations. 
	 * 			
	 * @note	It's mostly useful when you need to allocate something temporarily on the heap,
	 * 			usually something that gets allocated and freed within the same function.
	 * 			
	 *			Each allocation comes with a pretty hefty 4 byte memory overhead, so don't use it for small allocations. 
	 *			
	 *			This class is thread safe but you cannot allocate on one thread and deallocate on another. Threads will keep
	 *			separate stacks internally. Make sure to call beginThread/endThread for any thread this stack is used on.
	 */
	class MemStack
	{
	public:
		/**
		 * @brief	Sets up the stack with the currently active thread. You need to call this
		 * 			on any thread before doing any allocations or deallocations 
		 */
		static CM_UTILITY_EXPORT void beginThread();

		/**
		 * @brief	Cleans up the stack for the current thread. You may not perform any allocations or deallocations
		 * 			after this is called, unless you call beginThread again.
		 */
		static CM_UTILITY_EXPORT void endThread();

		static CM_UTILITY_EXPORT UINT8* alloc(UINT32 numBytes);
		static CM_UTILITY_EXPORT void deallocLast(UINT8* data);

	private:
		static CM_THREADLOCAL MemStackInternal<1024 * 1024>* ThreadMemStack;
	};

	CM_UTILITY_EXPORT inline UINT8* stackAlloc(UINT32 numBytes);

	template<class T>
	T* stackAlloc()
	{
		return (T*)MemStack::alloc(sizeof(T));
	}

	template<class T>
	T* stackAllocN(UINT32 count)
	{
		return (T*)MemStack::alloc(sizeof(T) * count);
	}

	template<class T>
	T* stackConstructN(UINT32 count)
	{
		T* data = stackAllocN<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T;

		return data;
	}

	template<class T>
	void stackDestruct(T* data)
	{
		data->~T();

		MemStack::deallocLast((UINT8*)data);
	}

	template<class T>
	void stackDestructN(T* data, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			data[i].~T();

		MemStack::deallocLast((UINT8*)data);
	}

	CM_UTILITY_EXPORT inline void stackDeallocLast(void* data);
}
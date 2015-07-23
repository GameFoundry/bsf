#pragma once

#include <stack>
#include <assert.h>
#include "BsStdHeaders.h"
#include "BsThreadDefines.h"

namespace BansheeEngine
{
	/**
	 * @brief	Describes a memory stack of a certain block capacity. See "MemoryStack" for more information.
	 *
	 *  @tparam	BlockCapacity Minimum size of a block. Larger blocks mean less memory allocations, but also potentially
	 * 			more wasted memory. If an allocation requests more bytes than BlockCapacity, first largest multiple is
	 * 			used instead.
	 */
	template <int BlockCapacity = 1024 * 1024>
	class MemStackInternal
	{
	private:

		/**
		 * @brief	A single block of memory of "BlockCapacity" size. A pointer to the first
		 * 			free address is stored, and a remaining size. 
		 */
		class MemBlock
		{
		public:
			MemBlock(UINT32 size)
				:mData(nullptr), mFreePtr(0), mSize(size), 
				mNextBlock(nullptr), mPrevBlock(nullptr)
			{ }

			~MemBlock()
			{ }

			/**
			 * @brief	Returns the first free address and increments the free pointer.
			 * 			Caller needs to ensure the remaining block size is adequate before
			 * 			calling.
			 */
			UINT8* alloc(UINT32 amount)
			{
				UINT8* freePtr = &mData[mFreePtr];
				mFreePtr += amount;

				return freePtr;
			}

			/**
			 * @brief	Deallocates the provided pointer. Deallocation must happen in opposite order
			 * 			from allocation otherwise corruption will occur.
			 * 			
			 * @note	Pointer to "data" isn't actually needed, but is provided for debug purposes in order to more
			 * 			easily track out-of-order deallocations.
			 */
			void dealloc(UINT8* data, UINT32 amount)
			{
				mFreePtr -= amount;
				assert((&mData[mFreePtr]) == data && "Out of order stack deallocation detected. Deallocations need to happen in order opposite of allocations.");
			}

			UINT8* mData;
			UINT32 mFreePtr;
			UINT32 mSize;
			MemBlock* mNextBlock;
			MemBlock* mPrevBlock;
		};

	public:
		MemStackInternal()
			:mFreeBlock(nullptr)
		{
			mFreeBlock = allocBlock(BlockCapacity);
		}

		~MemStackInternal()
		{
			assert(mFreeBlock->mFreePtr == 0 && "Not all blocks were released before shutting down the stack allocator.");

			MemBlock* curBlock = mFreeBlock;
			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->mNextBlock;
				deallocBlock(curBlock);

				curBlock = nextBlock;
			}
		}

		/**
		 * @brief	Allocates the given amount of memory on the stack.
		 *
		 * @param	amount	The amount to allocate in bytes.
		 *
		 * @note	Allocates the memory in the currently active block if it is large enough,
		 * 			otherwise a new block is allocated. If the allocation is larger than
		 * 			default block size a separate block will be allocated only for that allocation,
		 * 			making it essentially a slower heap allocator.
		 * 			
		 *			Each allocation comes with a 4 byte overhead.
		 */
		UINT8* alloc(UINT32 amount)
		{
			amount += sizeof(UINT32);

			UINT32 freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
			if(amount > freeMem)
				allocBlock(amount);

			UINT8* data = mFreeBlock->alloc(amount);

			UINT32* storedSize = reinterpret_cast<UINT32*>(data);
			*storedSize = amount;

			return data + sizeof(UINT32);
		}

		/**
		 * @brief	Deallocates the given memory. Data must be deallocated in opposite
		 * 			order then when it was allocated.
		 */
		void dealloc(UINT8* data)
		{
			data -= sizeof(UINT32);

			UINT32* storedSize = reinterpret_cast<UINT32*>(data);
			mFreeBlock->dealloc(data, *storedSize);

			if (mFreeBlock->mFreePtr == 0)
			{
				MemBlock* emptyBlock = mFreeBlock;

				if (emptyBlock->mPrevBlock != nullptr)
					mFreeBlock = emptyBlock->mPrevBlock;

				// Merge with next block
				if (emptyBlock->mNextBlock != nullptr)
				{
					UINT32 totalSize = emptyBlock->mSize + emptyBlock->mNextBlock->mSize;

					if (emptyBlock->mPrevBlock != nullptr)
						emptyBlock->mPrevBlock->mNextBlock = nullptr;
					else
						mFreeBlock = nullptr;

					deallocBlock(emptyBlock->mNextBlock);
					deallocBlock(emptyBlock);

					allocBlock(totalSize);
				}
			}
		}

	private:
		MemBlock* mFreeBlock;

		/**
		 * @brief	Allocates a new block of memory using a heap allocator. Block will never be 
		 * 			smaller than "BlockCapacity" no matter the "wantedSize".
		 */
		MemBlock* allocBlock(UINT32 wantedSize)
		{
			UINT32 blockSize = BlockCapacity;
			if(wantedSize > blockSize)
				blockSize = wantedSize;

			MemBlock* newBlock = nullptr;
			MemBlock* curBlock = mFreeBlock;

			while (curBlock != nullptr)
			{
				MemBlock* nextBlock = curBlock->mNextBlock;
				if (nextBlock->mSize >= blockSize)
				{
					newBlock = nextBlock;
					break;
				}

				curBlock = nextBlock;
			}

			if (newBlock == nullptr)
			{
				UINT8* data = (UINT8*)reinterpret_cast<UINT8*>(bs_alloc(blockSize + sizeof(MemBlock)));
				newBlock = new (data)MemBlock(blockSize);
				data += sizeof(MemBlock);

				newBlock->mData = data;
				newBlock->mPrevBlock = mFreeBlock;

				if (mFreeBlock != nullptr)
					mFreeBlock->mNextBlock = newBlock;
			}

			mFreeBlock = newBlock;
			return newBlock;
		}

		/**
		 * @brief	Deallocates a block of memory.
		 */
		void deallocBlock(MemBlock* block)
		{
			block->~MemBlock();
			bs_free(block);
		}
	};

	/**
	 * @brief	One of the fastest, but also very limiting type of allocator. All deallocations
	 * 			must happen in opposite order from allocations. 
	 * 			
	 * @note	It's mostly useful when you need to allocate something temporarily on the heap,
	 * 			usually something that gets allocated and freed within the same method.
	 * 			
	 *			Each allocation comes with a pretty hefty 4 byte memory overhead, so don't use it for small allocations. 
	 *			
	 *			Thread safe. But you cannot allocate on one thread and deallocate on another. Threads will keep
	 *			separate stacks internally. Make sure to call beginThread/endThread for any thread this stack is used on.
	 */
	class MemStack
	{
	public:
		/**
		 * @brief	Sets up the stack with the currently active thread. You need to call this
		 * 			on any thread before doing any allocations or deallocations 
		 */
		static BS_UTILITY_EXPORT void beginThread();

		/**
		 * @brief	Cleans up the stack for the current thread. You may not perform any allocations or deallocations
		 * 			after this is called, unless you call beginThread again.
		 */
		static BS_UTILITY_EXPORT void endThread();

		/**
		 * @copydoc	MemoryStackInternal::alloc
		 */
		static BS_UTILITY_EXPORT UINT8* alloc(UINT32 numBytes);

		/**
		 * @copydoc	MemoryStackInternal::dealloc
		 */
		static BS_UTILITY_EXPORT void deallocLast(UINT8* data);

	private:
		static BS_THREADLOCAL MemStackInternal<1024 * 1024>* ThreadMemStack;
	};

	/**
	 * @copydoc	MemoryStackInternal::alloc
	 */
	BS_UTILITY_EXPORT inline void* bs_stack_alloc(UINT32 numBytes);

	/**
	 * @brief	Allocates enough memory to hold the specified type, on the stack, but
	 * 			does not initialize the object. 
	 *
	 * @see		MemoryStackInternal::alloc()
	 */
	template<class T>
	T* bs_stack_alloc()
	{
		return (T*)MemStack::alloc(sizeof(T));
	}

	/**
	 * @brief	Allocates enough memory to hold N objects of the specified type, 
	 * 			on the stack, but does not initialize the objects. 
	 *
	 * @see		MemoryStackInternal::alloc()
	 */
	template<class T>
	T* bs_stack_alloc(UINT32 count)
	{
		return (T*)MemStack::alloc(sizeof(T) * count);
	}

	/**
	 * @brief	Allocates enough memory to hold the specified type, on the stack, 
	 * 			and constructs the object.
	 *
	 * @see		MemoryStackInternal::alloc()
	 */
	template<class T>
	T* bs_stack_new(UINT32 count = 0)
	{
		T* data = bs_stack_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T;

		return data;
	}

	/**
	 * @brief	Allocates enough memory to hold the specified type, on the stack, 
	 * 			and constructs the object.
	 *
	 * @see		MemoryStackInternal::alloc()
	 */
	template<class T, class... Args>
	T* bs_stack_new(Args &&...args, UINT32 count = 0)
	{
		T* data = bs_stack_alloc<T>(count);

		for(unsigned int i = 0; i < count; i++)
			new ((void*)&data[i]) T(std::forward<Args>(args)...);

		return data;
	}

	/**
	 * @brief	Destructs and deallocates last allocated entry currently located on stack.
	 *
	 * @see		MemoryStackInternal::dealloc()
	 */
	template<class T>
	void bs_stack_delete(T* data)
	{
		data->~T();

		MemStack::deallocLast((UINT8*)data);
	}

	/**
	 * @brief	Destructs an array of objects and deallocates last allocated 
	 * 			entry currently located on stack.
	 *
	 * @see		MemoryStackInternal::dealloc()
	 */
	template<class T>
	void bs_stack_delete(T* data, UINT32 count)
	{
		for(unsigned int i = 0; i < count; i++)
			data[i].~T();

		MemStack::deallocLast((UINT8*)data);
	}

	/**
	 * @copydoc	MemoryStackInternal::dealloc()
	 */
	BS_UTILITY_EXPORT inline void bs_stack_free(void* data);

	/**
	* @brief	Allows use of a stack allocator by using normal new/delete/free/dealloc operators.
	* 			
	* @see		MemStack
	*/
	class StackAlloc
	{ };

	/**
	* @brief	Specialized memory allocator implementations that allows use of a 
	* 			stack allocator in normal new/delete/free/dealloc operators.
	* 			
	* @see		MemStack
	*/
	template<>
	class MemoryAllocator<StackAlloc> : public MemoryAllocatorBase
	{
	public:
		static inline void* allocate(size_t bytes)
		{
			return bs_stack_alloc((UINT32)bytes);
		}

		static inline void* allocateArray(size_t bytes, UINT32 count)
		{
			return bs_stack_alloc((UINT32)(bytes * count));
		}

		static inline void free(void* ptr)
		{
			bs_stack_free(ptr);
		}

		static inline void freeArray(void* ptr, UINT32 count)
		{
			bs_stack_free(ptr);
		}
	};
}
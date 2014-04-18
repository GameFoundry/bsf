#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotFramework
{
	/**
	 * @brief	Frame allocator. Performs very fast allocations but can only free all of its memory at once.
	 * 			Perfect for allocations that last just a single frame.
	 * 			
	 * @note	Not thread safe with an exception. "alloc" and "clear" methods need to be called from the same thread.
	 * 			"dealloc" is thread safe and can be called from any thread.
	 */
	class CM_UTILITY_EXPORT FrameAlloc
	{
	private:
		class MemBlock
		{
		public:
			MemBlock(UINT32 size);
			~MemBlock();

			UINT8* alloc(UINT8 amount);
			void clear();

			UINT8* mData;
			UINT32 mFreePtr;
			UINT32 mSize;
		};

	public:
		FrameAlloc(UINT32 blockSize = 1024 * 1024);
		~FrameAlloc();

		/**
		 * @brief	Allocates a new block of memory of the specified size.
		 *
		 * @param	amount	Amount of memory to allocate, in bytes.
		 * 					
		 * @note	Not thread safe.
		 */
		UINT8* alloc(UINT32 amount);

		/**
		 * @brief	Deallocates a previously allocated block of memory.
		 *
		 * @note	No deallocation is actually done here. This method is only used for debug purposes
		 * 			so it is easier to track down memory leaks and corruption.
		 * 			
		 *			Thread safe.
		 */
		void dealloc(UINT8* data);

		/**
		 * @brief	Deallocates all allocated memory.
		 * 			
		 * @note	Not thread safe.
		 */
		void clear();

	private:
		UINT32 mBlockSize;
		Vector<MemBlock*>::type mBlocks;
		MemBlock* mFreeBlock;
		std::atomic<UINT32> mTotalAllocBytes;

		MemBlock* allocBlock(UINT32 wantedSize);
		void deallocBlock(MemBlock* block);
	};
}
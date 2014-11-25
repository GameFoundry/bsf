#include "BsFrameAlloc.h"
#include "BsException.h"

namespace BansheeEngine
{
	FrameAlloc::MemBlock::MemBlock(UINT32 size)
		:mData(nullptr), mFreePtr(0), mSize(size)
	{ }

	FrameAlloc::MemBlock::~MemBlock()
	{ }

	UINT8* FrameAlloc::MemBlock::alloc(UINT32 amount)
	{
		UINT8* freePtr = &mData[mFreePtr];
		mFreePtr += amount;

		return freePtr;
	}

	void FrameAlloc::MemBlock::clear()
	{
		mFreePtr = 0;
	}

	FrameAlloc::FrameAlloc(UINT32 blockSize)
		:mTotalAllocBytes(0), mFreeBlock(nullptr), mBlockSize(blockSize)
	{
		allocBlock(mBlockSize);
	}

	FrameAlloc::~FrameAlloc()
	{
		for(auto& block : mBlocks)
			deallocBlock(block);
	}

	UINT8* FrameAlloc::alloc(UINT32 amount)
	{
#if BS_DEBUG_MODE
		assert(mOwnerThread == BS_THREAD_CURRENT_ID && "Frame allocator called from invalid thread.");

		amount += sizeof(UINT32) * 2;
#endif

		UINT32 freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
		if(amount > freeMem)
			allocBlock(amount);

		UINT8* data = mFreeBlock->alloc(amount);

#if BS_DEBUG_MODE
		mTotalAllocBytes += amount;

		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		*storedSize = amount;

		UINT32* storedId = reinterpret_cast<UINT32*>(data + sizeof(UINT32));
		*storedId = mAllocId;

		mActiveAllocs.insert(mAllocId);
		mAllocId++;

		return data + sizeof(UINT32) * 2;
#else
		return data;
#endif
	}

	void FrameAlloc::dealloc(UINT8* data)
	{
		// Dealloc is only used for debug and can be removed if needed. All the actual deallocation
		// happens in "clear"
			
#if BS_DEBUG_MODE
		data -= sizeof(UINT32) * 2;
		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		UINT32* storedId = reinterpret_cast<UINT32*>(data + sizeof(UINT32));
		mTotalAllocBytes -= *storedSize;
		mActiveAllocs.erase(*storedId);
#endif
	}

	void FrameAlloc::clear()
	{
#if BS_DEBUG_MODE
		assert(mOwnerThread == BS_THREAD_CURRENT_ID && "Frame allocator called from invalid thread.");

		if(mTotalAllocBytes.load() > 0)
			BS_EXCEPT(InvalidStateException, "Not all frame allocated bytes were properly released.");

		mAllocId = 0;
		mActiveAllocs.clear();
#endif

		// Merge all blocks into one
		UINT32 totalBytes = 0;
		for(auto& block : mBlocks)
		{
			totalBytes += block->mSize;
			deallocBlock(block);
		}

		mBlocks.clear();
			
		allocBlock(totalBytes);			
	}

	FrameAlloc::MemBlock* FrameAlloc::allocBlock(UINT32 wantedSize)
	{
		UINT32 blockSize = mBlockSize;
		if(wantedSize > blockSize)
			blockSize = wantedSize;

		UINT8* data = (UINT8*)reinterpret_cast<UINT8*>(bs_alloc(blockSize + sizeof(MemBlock)));
		MemBlock* newBlock = new (data) MemBlock(blockSize);
		data += sizeof(MemBlock);
		newBlock->mData = data;

		mBlocks.push_back(newBlock);
		mFreeBlock = newBlock; // If previous block had some empty space it is lost until next "clear"

		return newBlock;
	}

	void FrameAlloc::deallocBlock(MemBlock* block)
	{
		block->~MemBlock();
		bs_free(block);
	}
}
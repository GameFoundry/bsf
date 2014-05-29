#include "CmFrameAlloc.h"
#include "CmException.h"

namespace BansheeEngine
{
	FrameAlloc::MemBlock::MemBlock(UINT32 size)
		:mData(nullptr), mFreePtr(0), mSize(size)
	{ }

	FrameAlloc::MemBlock::~MemBlock()
	{ }

	UINT8* FrameAlloc::MemBlock::alloc(UINT8 amount)
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
		amount += sizeof(UINT32);
#endif

		UINT32 freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
		if(amount > freeMem)
			allocBlock(amount);

		UINT8* data = mFreeBlock->alloc(amount);

#if BS_DEBUG_MODE
		mTotalAllocBytes += amount;

		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		*storedSize = amount;

		return data + sizeof(UINT32);
#else
		return data;
#endif
	}

	void FrameAlloc::dealloc(UINT8* data)
	{
		// Dealloc is only used for debug and can be removed if needed. All the actual deallocation
		// happens in "clear"
			
#if BS_DEBUG_MODE
		data -= sizeof(UINT32);
		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		mTotalAllocBytes -= *storedSize;
#endif
	}

	void FrameAlloc::clear()
	{
#if BS_DEBUG_MODE
		if(mTotalAllocBytes.load() > 0)
			BS_EXCEPT(InvalidStateException, "Not all frame allocated bytes were properly released.");
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
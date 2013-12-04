#include "CmFrameAlloc.h"
#include "CmException.h"

namespace CamelotFramework
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
		amount += sizeof(UINT32);

		UINT32 freeMem = mFreeBlock->mSize - mFreeBlock->mFreePtr;
		if(amount > freeMem)
			allocBlock(amount);

		UINT8* data = mFreeBlock->alloc(amount);
		mTotalAllocBytes += amount;

		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		*storedSize = amount;

		return data + sizeof(UINT32);
	}

	void FrameAlloc::dealloc(UINT8* data)
	{
		// Dealloc is only used for debug and can be removed if needed. All the actual deallocation
		// happens in "clear"
			
		data -= sizeof(UINT32);
		UINT32* storedSize = reinterpret_cast<UINT32*>(data);
		mTotalAllocBytes -= *storedSize;
	}

	void FrameAlloc::clear()
	{
		if(mTotalAllocBytes.load() > 0)
			CM_EXCEPT(InvalidStateException, "Not all frame allocated bytes were properly released.");

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

		UINT8* data = (UINT8*)reinterpret_cast<UINT8*>(cm_alloc(blockSize + sizeof(MemBlock)));
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
		cm_free(block);
	}
}
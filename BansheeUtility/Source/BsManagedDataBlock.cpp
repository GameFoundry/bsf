#include "BsManagedDataBlock.h"
#include "BsException.h"

namespace BansheeEngine
{
	ManagedDataBlock::ManagedDataBlock(UINT8* data, UINT32 size, std::function<void(UINT8*)> deallocator)
		:mData(data), mSize(size), mManaged(false), mIsDataOwner(true), mDeallocator(deallocator)
	{ }

	ManagedDataBlock::ManagedDataBlock(UINT32 size)
		:mSize(size), mManaged(true), mIsDataOwner(true), mDeallocator(nullptr)
	{
		mData = (UINT8*)bs_alloc<ScratchAlloc>(size);
	}

	ManagedDataBlock::ManagedDataBlock(const ManagedDataBlock& source)
	{
		mData = source.mData;
		mSize = source.mSize;
		mManaged = source.mManaged;
		mDeallocator = source.mDeallocator;

		mIsDataOwner = true;
		source.mIsDataOwner = false;
	}

	ManagedDataBlock::~ManagedDataBlock()
	{
		if(mManaged && mIsDataOwner)
		{
			if(mDeallocator != nullptr)
				mDeallocator(mData);
			else
				bs_free<ScratchAlloc>(mData);
		}
	}
}
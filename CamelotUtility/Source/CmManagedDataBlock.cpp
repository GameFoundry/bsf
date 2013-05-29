#include "CmManagedDataBlock.h"
#include "CmException.h"

namespace CamelotFramework
{
	ManagedDataBlock::ManagedDataBlock(UINT8* data, UINT32 size)
		:mData(data), mSize(size), mManaged(false), mIsDataOwner(true)
	{ }

	ManagedDataBlock::ManagedDataBlock(UINT32 size)
		:mSize(size), mManaged(true), mIsDataOwner(true)
	{
		mData = (UINT8*)cm_alloc<ScratchAlloc>(size);
	}

	ManagedDataBlock::ManagedDataBlock(const ManagedDataBlock& source)
	{
		mData = source.mData;
		mSize = source.mSize;
		mManaged = source.mManaged;

		mIsDataOwner = true;
		source.mIsDataOwner = false;
	}

	ManagedDataBlock::~ManagedDataBlock()
	{
		if(mManaged && mIsDataOwner)
			cm_free<ScratchAlloc>(mData);
	}
}
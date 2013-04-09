#include "CmManagedDataBlock.h"
#include "CmException.h"

namespace CamelotEngine
{
	ManagedDataBlock::ManagedDataBlock(UINT8* data, UINT32 size)
		:mData(data), mSize(size), mManaged(false), mIsDataOwner(true)
	{ }

	ManagedDataBlock::ManagedDataBlock(UINT32 size)
		:mSize(size), mManaged(true), mIsDataOwner(true)
	{
		mData = CM_NEW_BYTES(size, ScratchAlloc);
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
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}
}
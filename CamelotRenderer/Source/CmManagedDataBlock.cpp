#include "CmManagedDataBlock.h"
#include "CmException.h"

namespace CamelotEngine
{
	ManagedDataBlock::ManagedDataBlock(UINT8* data, UINT32 size, bool managed)
		:mData(data), mSize(size), mManaged(managed), mIsDataOwner(true)
	{ }

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
			delete[] mData;
	}

	void ManagedDataBlock::destroy()
	{
		if(mManaged)
			CM_EXCEPT(InternalErrorException, "Trying to manually destroy managed data.");

		if(mData != nullptr)
			delete[] mData;

		mSize = 0;
		mData = nullptr;
	}
}
//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsManagedDataBlock.h"
#include "BsException.h"

namespace BansheeEngine
{
	ManagedDataBlock::ManagedDataBlock(UINT8* data, UINT32 size)
		:mData(data), mSize(size), mManaged(false), mIsDataOwner(true)
	{ }

	ManagedDataBlock::ManagedDataBlock(UINT32 size)
		:mSize(size), mManaged(true), mIsDataOwner(true)
	{
		mData = (UINT8*)bs_alloc(size);
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
			bs_free(mData);
	}
}
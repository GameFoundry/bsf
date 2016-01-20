//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsIndexBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderAPI.h"

namespace BansheeEngine 
{
	IndexBufferProperties::IndexBufferProperties(IndexType idxType, UINT32 numIndexes)
		:mIndexType(idxType), mNumIndexes(numIndexes)
	{
		switch (mIndexType)
		{
		case IT_16BIT:
			mIndexSize = sizeof(unsigned short);
			break;
		case IT_32BIT:
			mIndexSize = sizeof(unsigned int);
			break;
		}
	}

	IndexBufferCore::IndexBufferCore(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage)
		:HardwareBuffer(usage, false), mProperties(idxType, numIndexes)
	{ 
		mSizeInBytes = mProperties.mIndexSize * mProperties.mNumIndexes;
	}

    IndexBuffer::IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage) 
		: mUsage(usage), mProperties(idxType, numIndexes)
    {

	}

	SPtr<IndexBufferCore> IndexBuffer::getCore() const
	{
		return std::static_pointer_cast<IndexBufferCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> IndexBuffer::createCore() const
	{
		return HardwareBufferCoreManager::instance().createIndexBufferInternal(mProperties.mIndexType, mProperties.mNumIndexes, mUsage);
	}

	IndexBufferPtr IndexBuffer::create(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		return HardwareBufferManager::instance().createIndexBuffer(itype, numIndexes, usage);
	}
}
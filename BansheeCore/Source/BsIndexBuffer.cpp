#include "BsIndexBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
	IndexBufferCore::IndexBufferCore(GpuBufferUsage usage, bool useSystemMemory, const IndexBufferProperties& properties)
		:HardwareBuffer(usage, useSystemMemory), mProperties(properties)
	{ 
		mSizeInBytes = mProperties.mIndexSize * mProperties.mNumIndexes;
	}

    IndexBuffer::IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage, bool useSystemMemory) 
		:mUsage(usage), mUseSystemMemory(useSystemMemory)
    {
		mProperties.mIndexType = idxType;
		mProperties.mNumIndexes = numIndexes;

		switch (mProperties.mIndexType)
        {
        case IT_16BIT:
			mProperties.mIndexSize = sizeof(unsigned short);
            break;
        case IT_32BIT:
			mProperties.mIndexSize = sizeof(unsigned int);
            break;
        }
    }

	IndexBufferCore* IndexBuffer::getCore() const
	{
		return static_cast<IndexBufferCore*>(mCoreSpecific);
	}

	IndexBufferPtr IndexBuffer::create(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		return HardwareBufferManager::instance().createIndexBuffer(itype, numIndexes, usage);
	}
}
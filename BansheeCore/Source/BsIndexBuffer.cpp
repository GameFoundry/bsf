#include "BsIndexBuffer.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
    IndexBuffer::IndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage, bool useSystemMemory) 
        : HardwareBuffer(usage, useSystemMemory), mIndexType(idxType), mNumIndexes(numIndexes)
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

        mSizeInBytes = mIndexSize * mNumIndexes;
    }

    IndexBuffer::~IndexBuffer()
    {

    }

	IndexBufferPtr IndexBuffer::create(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		return HardwareBufferManager::instance().createIndexBuffer(itype, numIndexes, usage);
	}
}
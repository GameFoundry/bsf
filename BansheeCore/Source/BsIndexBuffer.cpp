//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
}
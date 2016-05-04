//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPixelBuffer.h"
#include "BsException.h"

namespace BansheeEngine 
{
    PixelBuffer::PixelBuffer(UINT32 width, UINT32 height, UINT32 depth,
            PixelFormat format,
            GpuBufferUsage usage, bool useSystemMemory):
        HardwareBuffer(usage, useSystemMemory),
        mWidth(width), mHeight(height), mDepth(depth),
        mFormat(format)
    {
        mRowPitch = mWidth;
        mSlicePitch = mHeight*mWidth;
		mSizeInBytes = mHeight*mWidth*PixelUtil::getNumElemBytes(mFormat);
    }

    PixelBuffer::~PixelBuffer()
    {
    }

    void* PixelBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
        assert(!isLocked() && "Cannot lock this buffer, it is already locked!");
        assert(offset == 0 && length == mSizeInBytes && "Cannot lock memory region, most lock box or entire buffer");
        
        PixelVolume myBox(0, 0, 0, mWidth, mHeight, mDepth);
        const PixelData &rv = lock(myBox, options);
        return rv.getData();
    }

    const PixelData& PixelBuffer::lock(const PixelVolume& lockBox, GpuLockOptions options)
    {
        // Lock the real buffer if there is no shadow buffer 
        mCurrentLock = lockImpl(lockBox, options);
        mIsLocked = true;

        return mCurrentLock;
    }
    
    void* PixelBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
		BS_EXCEPT(InternalErrorException, "lockImpl(offset,length) is not valid for PixelBuffers and should never be called");
		return nullptr;
    }

	void PixelBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		// TODO
		BS_EXCEPT(NotImplementedException, "Reading a byte range is not implemented. Use blitToMemory.");
	}

	void PixelBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
		// TODO
		BS_EXCEPT(NotImplementedException, "Writing a byte range is not implemented. Use blitFromMemory.");
	}
}

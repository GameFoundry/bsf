/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#include "CmHardwarePixelBuffer.h"
#include "CmException.h"

namespace CamelotEngine 
{
  
    //-----------------------------------------------------------------------------    
    HardwarePixelBuffer::HardwarePixelBuffer(UINT32 width, UINT32 height, UINT32 depth,
            PixelFormat format,
            HardwareBuffer::Usage usage, bool useSystemMemory):
        HardwareBuffer(usage, useSystemMemory),
        mWidth(width), mHeight(height), mDepth(depth),
        mFormat(format)
    {
        // Default
        mRowPitch = mWidth;
        mSlicePitch = mHeight*mWidth;
		mSizeInBytes = mHeight*mWidth*PixelUtil::getNumElemBytes(mFormat);
    }
    
    //-----------------------------------------------------------------------------    
    HardwarePixelBuffer::~HardwarePixelBuffer()
    {
    }
    
    //-----------------------------------------------------------------------------    
    void* HardwarePixelBuffer::lock(UINT32 offset, UINT32 length, LockOptions options)
    {
        assert(!isLocked() && "Cannot lock this buffer, it is already locked!");
        assert(offset == 0 && length == mSizeInBytes && "Cannot lock memory region, most lock box or entire buffer");
        
        Box myBox(0, 0, 0, mWidth, mHeight, mDepth);
        const PixelData &rv = lock(myBox, options);
        return rv.data;
    }
    
    //-----------------------------------------------------------------------------    
    const PixelData& HardwarePixelBuffer::lock(const Box& lockBox, LockOptions options)
    {
        // Lock the real buffer if there is no shadow buffer 
        mCurrentLock = lockImpl(lockBox, options);
        mIsLocked = true;

        return mCurrentLock;
    }
    
    //-----------------------------------------------------------------------------    
    const PixelData& HardwarePixelBuffer::getCurrentLock() 
	{ 
        assert(isLocked() && "Cannot get current lock: buffer not locked");
        
        return mCurrentLock; 
    }
    
    //-----------------------------------------------------------------------------    
    /// Internal implementation of lock()
    void* HardwarePixelBuffer::lockImpl(UINT32 offset, UINT32 length, LockOptions options)
    {
		CM_EXCEPT(InternalErrorException, "lockImpl(offset,length) is not valid for PixelBuffers and should never be called");
    }

    //-----------------------------------------------------------------------------    

    void HardwarePixelBuffer::blit(const HardwarePixelBufferPtr &src, const Box &srcBox, const Box &dstBox)
	{
		if(isLocked() || src->isLocked())
		{
			CM_EXCEPT(InternalErrorException,
				"Source and destination buffer may not be locked!");
		}
		if(src.get() == this)
		{
			CM_EXCEPT(InternalErrorException,
                "Source must not be the same object") ;
		}
		const PixelData &srclock = src->lock(srcBox, HBL_READ_ONLY);

		LockOptions method = HBL_READ_WRITE;
		if(dstBox.left == 0 && dstBox.top == 0 && dstBox.front == 0 &&
		   dstBox.right == mWidth && dstBox.bottom == mHeight &&
		   dstBox.back == mDepth)
			// Entire buffer -- we can discard the previous contents
			method = HBL_WRITE_ONLY_DISCARD;
			
		const PixelData &dstlock = lock(dstBox, method);
		if(dstlock.getWidth() != srclock.getWidth() ||
        	dstlock.getHeight() != srclock.getHeight() ||
        	dstlock.getDepth() != srclock.getDepth())
		{
			// Scaling desired
			PixelUtil::scale(srclock, dstlock);
		}
		else
		{
			// No scaling needed
			PixelUtil::bulkPixelConversion(srclock, dstlock);
		}

		unlock();
		src->unlock();
	}
    //-----------------------------------------------------------------------------       
    void HardwarePixelBuffer::blit(const HardwarePixelBufferPtr &src)
    {
        blit(src, 
            Box(0,0,0,src->getWidth(),src->getHeight(),src->getDepth()), 
            Box(0,0,0,mWidth,mHeight,mDepth)
        );
    }
    //-----------------------------------------------------------------------------    
	void HardwarePixelBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		// TODO
		CM_EXCEPT(NotImplementedException,
				"Reading a byte range is not implemented. Use blitToMemory.");
	}
	//-----------------------------------------------------------------------------    

	void HardwarePixelBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer)
	{
		// TODO
		CM_EXCEPT(NotImplementedException,
				"Writing a byte range is not implemented. Use blitFromMemory.");
	}
}

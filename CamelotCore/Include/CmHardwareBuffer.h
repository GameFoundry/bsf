/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

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
#ifndef __HardwareBuffer__
#define __HardwareBuffer__

// Precompiler options
#include "CmPrerequisites.h"
#include "CmCommonEnums.h"

namespace CamelotFramework 
{
	/** \addtogroup Core
	*  @{
	*/
	/** \addtogroup RenderSystem
	*  @{
	*/
	/** Abstract class defining common features of hardware buffers.
    @remarks
 	    A 'hardware buffer' is any area of memory held outside of core system ram,
	    and in our case refers mostly to video ram, although in theory this class
	    could be used with other memory areas such as sound card memory, custom
	    coprocessor memory etc.
    @par
 	    This reflects the fact that memory held outside of main system RAM must 
	    be interacted with in a more formal fashion in order to promote
	    cooperative and optimal usage of the buffers between the various 
	    processing units which manipulate them.
    @par
 	    This abstract class defines the core interface which is common to all
	    buffers, whether it be vertex buffers, index buffers, texture memory
	    or framebuffer memory etc.
	@par
		Buffers have the ability to be 'shadowed' in system memory, this is because
		the kinds of access allowed on hardware buffers is not always as flexible as
		that allowed for areas of system memory - for example it is often either 
		impossible, or extremely undesirable from a performance standpoint to read from
		a hardware buffer; when writing to hardware buffers, you should also write every
		byte and do it sequentially. In situations where this is too restrictive, 
		it is possible to create a hardware, write-only buffer (the most efficient kind) 
		and to back it with a system memory 'shadow' copy which can be read and updated arbitrarily.
		Ogre handles synchronising this buffer with the real hardware buffer (which should still be
		created with the HBU_DYNAMIC flag if you intend to update it very frequently). Whilst this
		approach does have it's own costs, such as increased memory overhead, these costs can 
		often be outweighed by the performance benefits of using a more hardware efficient buffer.
		You should look for the 'useShadowBuffer' parameter on the creation methods used to create
		the buffer of the type you require (see HardwareBufferManager) to enable this feature.
    */
	class CM_EXPORT HardwareBuffer
    {
	    protected:
		    UINT32 mSizeInBytes;
		    GpuBufferUsage mUsage;
		    bool mIsLocked;
			UINT32 mLockStart;
			UINT32 mLockSize;
			bool mSystemMemory;
    		
            /// Internal implementation of lock()
		    virtual void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;
            /// Internal implementation of unlock()
		    virtual void unlockImpl(void) = 0;

    public:
		    /// Constructor, to be called by HardwareBufferManager only
            HardwareBuffer(GpuBufferUsage usage, bool systemMemory) 
				: mUsage(usage), mIsLocked(false), mSystemMemory(systemMemory)
            {  }
            virtual ~HardwareBuffer() {}
		    /** Lock the buffer for (potentially) reading / writing.
		    @param offset The byte offset from the start of the buffer to lock
		    @param length The size of the area to lock, in bytes
		    @param options Locking options
		    @returns Pointer to the locked memory
		    */
		    virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options)
            {
                assert(!isLocked() && "Cannot lock this buffer, it is already locked!");
                void* ret = lockImpl(offset, length, options);
                mIsLocked = true;

				mLockStart = offset;
				mLockSize = length;
                return ret;
            }

            /** Lock the entire buffer for (potentially) reading / writing.
		    @param options Locking options
		    @returns Pointer to the locked memory
            */
            void* lock(GpuLockOptions options)
            {
                return this->lock(0, mSizeInBytes, options);
            }
		    /** Releases the lock on this buffer. 
            @remarks 
                Locking and unlocking a buffer can, in some rare circumstances such as 
                switching video modes whilst the buffer is locked, corrupt the 
                contents of a buffer. This is pretty rare, but if it occurs, 
                this method will throw an exception, meaning you
                must re-upload the data.
            @par
                Note that using the 'read' and 'write' forms of updating the buffer does not
                suffer from this problem, so if you want to be 100% sure your
                data will not be lost, use the 'read' and 'write' forms instead.
            */
		    virtual void unlock(void)
            {
                assert(isLocked() && "Cannot unlock this buffer, it is not locked!");

                unlockImpl();
                mIsLocked = false;
            }

            /** Reads data from the buffer and places it in the memory pointed to by pDest.
		    @param offset The byte offset from the start of the buffer to read
		    @param length The size of the area to read, in bytes
            @param pDest The area of memory in which to place the data, must be large enough to 
                accommodate the data!
            */
            virtual void readData(UINT32 offset, UINT32 length, void* pDest) = 0;
            /** Writes data to the buffer from an area of system memory; note that you must
                ensure that your buffer is big enough.
		    @param offset The byte offset from the start of the buffer to start writing
		    @param length The size of the data to write to, in bytes
            @param pSource The source of the data to be written
			@param discardWholeBuffer If true, this allows the driver to discard the entire buffer when writing,
				such that stalls can be avoided. Always use when you can.
			@param noOverwrite If true, you are guaranteeing to the driver that you will not write to any area of the
				buffer the GPU is currently using. This will avoid stalls. Be aware that guaranteeing that
				something isn't used on the GPU is problematic.
            */
            virtual void writeData(UINT32 offset, UINT32 length, const void* pSource,
					BufferWriteType writeFlags = BufferWriteType::Normal) = 0;

			/** Copy data from another buffer into this one.
			@remarks
				Note that the source buffer must not be created with the
                usage HBU_WRITE_ONLY otherwise this will fail. 
			@param srcBuffer The buffer from which to read the copied data
			@param srcOffset Offset in the source buffer at which to start reading
			@param dstOffset Offset in the destination buffer to start writing
			@param length Length of the data to copy, in bytes.
			@param discardWholeBuffer If true, will discard the entire contents of this buffer before copying
			*/
			virtual void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
				UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false)
			{
				const void *srcData = srcBuffer.lock(
					srcOffset, length, GBL_READ_ONLY);
				this->writeData(dstOffset, length, srcData, discardWholeBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);
				srcBuffer.unlock();
			}

			/** Copy all data from another buffer into this one. 
			@remarks
				Normally these buffers should be of identical size, but if they're
				not, the routine will use the smallest of the two sizes.
			*/
			virtual void copyData(HardwareBuffer& srcBuffer)
			{
				UINT32 sz = std::min(getSizeInBytes(), srcBuffer.getSizeInBytes()); 
				copyData(srcBuffer, 0, 0, sz, true);
			}
			
            /// Returns the size of this buffer in bytes
            UINT32 getSizeInBytes(void) const { return mSizeInBytes; }
            /// Returns the Usage flags with which this buffer was created
            GpuBufferUsage getUsage(void) const { return mUsage; }
			/// Returns whether this buffer is held in system memory
			bool isSystemMemory(void) const { return mSystemMemory; }
            /// Returns whether or not this buffer is currently locked.
            bool isLocked(void) const { 
                return mIsLocked; 
            }	
    };
	/** @} */
	/** @} */
}
#endif



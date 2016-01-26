#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Abstract class defining common features of hardware buffers. Hardware buffers usually
	 *			represent areas of memory the GPU or the driver can access directly.
	 *
	 * @note	Be aware that reading from non-system memory hardware buffers is usually slow and should be avoided.
	 */
	class BS_CORE_EXPORT HardwareBuffer
    {
    public:
        virtual ~HardwareBuffer() {}

		/**
		 * @brief	Locks a portion of the buffer and returns pointer to the locked area.
		 *			You must call "unlock" when done.
		 *
		 * @param	offset	Offset in bytes from which to lock the buffer.
		 * @param	length	Length of the area you want to lock, in bytes.
		 * @param	options	Signifies what you want to do with the returned pointer.
		 *					Caller must ensure not to do anything he hasn't requested.
		 *					(e.g. don't try to read from the buffer unless you requested
		 *					it here).
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

		/**
		 * @brief	Locks the entire buffer and returns pointer to the locked area.
		 *			You must call "unlock" when done.
		 *
		 * @param	options	Signifies what you want to do with the returned pointer.
		 *					Caller must ensure not to do anything he hasn't requested.
		 *					(e.g. don't try to read from the buffer unless you requested
		 *					it here).
		 */
        void* lock(GpuLockOptions options)
        {
            return this->lock(0, mSizeInBytes, options);
        }

		/**
		 * @brief	Releases the lock on this buffer. 
		 */
		virtual void unlock()
        {
            assert(isLocked() && "Cannot unlock this buffer, it is not locked!");

            unlockImpl();
            mIsLocked = false;
        }

		/**
		 * @brief	Reads data from a portion of the buffer and copies it to the destination
		 *			buffer. Caller must ensure destination buffer is large enough.
		 *
		 * @param	offset	Offset in bytes from which to copy the data.
		 * @param	length	Length of the area you want to copy, in bytes.
		 * @param	dest	Destination buffer large enough to store the read data.
		 */
        virtual void readData(UINT32 offset, UINT32 length, void* dest) = 0;

		/**
		 * @brief	Writes data into a portion of the buffer from the source memory. 
		 *
		 * @param	offset		Offset in bytes from which to copy the data.
		 * @param	length		Length of the area you want to copy, in bytes.
		 * @param	source		Source buffer containing the data to write.
		 * @param	writeFlags	Optional write flags that may affect performance.
		 */
        virtual void writeData(UINT32 offset, UINT32 length, const void* source,
				BufferWriteType writeFlags = BufferWriteType::Normal) = 0;

		/**
		 * @brief	Copies data from a specific portion of the source buffer into a specific portion
		 *			of this buffer.
		 *
		 * @param	srcBuffer			Buffer to copy from.
		 * @param	srcOffset			Offset into the source buffer to start copying from, in bytes.
		 * @param	dstOffset			Offset into this buffer to start copying to, in bytes.
		 * @param	length				Size of the data to copy, in bytes.
		 * @param	discardWholeBuffer	Specify true if the data in the current buffer can be entirely discarded. This
		 *								may improve performance.
		 */
		virtual void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false)
		{
			const void *srcData = srcBuffer.lock(
				srcOffset, length, GBL_READ_ONLY);
			this->writeData(dstOffset, length, srcData, discardWholeBuffer ? BufferWriteType::Discard : BufferWriteType::Normal);
			srcBuffer.unlock();
		}

		/**
		 * @brief	Copy data from the provided buffer into this buffer. If buffers
		 *			are not the same size, smaller size will be used.
		 */
		virtual void copyData(HardwareBuffer& srcBuffer)
		{
			UINT32 sz = std::min(getSizeInBytes(), srcBuffer.getSizeInBytes()); 
			copyData(srcBuffer, 0, 0, sz, true);
		}
			
		/**
		 * @brief	Returns the size of this buffer in bytes.
		 */
        UINT32 getSizeInBytes(void) const { return mSizeInBytes; }

		/**
		 * @brief	Returns the Usage flags with which this buffer was created.
		 */
        GpuBufferUsage getUsage() const { return mUsage; }

		/**
		 * @brief	Returns whether this buffer is held in system memory.
		 */
		bool isSystemMemory() const { return mSystemMemory; }

		/**
		 * @brief	Returns whether or not this buffer is currently locked.
		 */
        bool isLocked() const { return mIsLocked; }	

	protected:
		friend class HardwareBufferManager;

		/**
		 * @brief	Constructs a new buffer.
		 *
		 * @param	usage			Determines most common usage of the buffer. Usually has effect on what
		 *							type of memory will be buffer allocated in but that depends on render API.
		 *							Specify dynamic if you plan on modifying it often, static otherwise.
		 * @param	systemMemory	If enabled the the buffer will be kept in the system memory. System memory
		 *							buffers are often used as a source or destination for copies from/to other
		 *							buffers. Some APIs don't allow reading from non-system memory buffers.
		 */
		HardwareBuffer(GpuBufferUsage usage, bool systemMemory)
			: mUsage(usage), mIsLocked(false), mSystemMemory(systemMemory)
		{  }

		/**
		 * @copydoc	lock
		 */
		virtual void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;

		/**
		 * @copydoc	unlock
		 */
		virtual void unlockImpl() = 0;

	protected:
		UINT32 mSizeInBytes;
		GpuBufferUsage mUsage;
		bool mIsLocked;
		UINT32 mLockStart;
		UINT32 mLockSize;
		bool mSystemMemory;
    };
}



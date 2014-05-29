#pragma once

#include "BsGLPrerequisites.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{ 
    class BS_RSGL_EXPORT GLIndexBuffer : public IndexBuffer
    {
    public:
        ~GLIndexBuffer();
        /** See HardwareBuffer. */
        void readData(UINT32 offset, UINT32 length, void* pDest);
        /** See HardwareBuffer. */
        void writeData(UINT32 offset, UINT32 length, 
			const void* pSource, BufferWriteType writeFlags = BufferWriteType::Normal);

        GLuint getGLBufferId(void) const { return mBufferId; }

	protected:
		friend class GLHardwareBufferManager;

		GLIndexBuffer(IndexType idxType, UINT32 numIndexes, 
			GpuBufferUsage usage); 

		/** See HardwareBuffer. */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);
		/** See HardwareBuffer. */
		void unlockImpl(void);

		/**
		 * @copydoc IndexBuffer::initialize_internal()
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc IndexBuffer::destroy_internal()
		 */
		void destroy_internal();

	private:
		GLuint mBufferId;
    };
}
#pragma once

#include "BsGLPrerequisites.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{ 
	/**
	 * @brief	OpenGL implementation of an index buffer.
	 */
    class BS_RSGL_EXPORT GLIndexBuffer : public IndexBuffer
    {
    public:
        ~GLIndexBuffer();

		/**
		 * @copydoc IndexBuffer::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc IndexBuffer::writeData
		 */
        void writeData(UINT32 offset, UINT32 length, const void* pSource, 
			BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @brief	Returns internal OpenGL index buffer handle.
		 */
        GLuint getGLBufferId() const { return mBufferId; }

	protected:
		friend class GLHardwareBufferManager;

		GLIndexBuffer(IndexType idxType, UINT32 numIndexes, 
			GpuBufferUsage usage); 

		/**
		 * @copydoc IndexBuffer::initialize_internal
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc IndexBuffer::destroy_internal
		 */
		void destroy_internal();

		/**
		 * @copydoc IndexBuffer::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc IndexBuffer::unlockImpl
		 */
		void unlockImpl();

	private:
		GLuint mBufferId;
    };
}
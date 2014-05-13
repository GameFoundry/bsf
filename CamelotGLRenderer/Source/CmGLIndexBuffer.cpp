#include "CmGLIndexBuffer.h"
#include "CmGLHardwareBufferManager.h"
#include "CmException.h"

namespace BansheeEngine 
{
    GLIndexBuffer::GLIndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage)
        : IndexBuffer(idxType, numIndexes, usage, false)
    {  }

    GLIndexBuffer::~GLIndexBuffer()
    {    }

	void GLIndexBuffer::initialize_internal()
	{
		glGenBuffers(1, &mBufferId );

		if (!mBufferId)
		{
			CM_EXCEPT(InternalErrorException, 
				"Cannot create GL index buffer");
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);

		// Initialise buffer and set usage
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, 
			GLHardwareBufferManager::getGLUsage(mUsage));

		IndexBuffer::initialize_internal();
	}

	void GLIndexBuffer::destroy_internal()
	{
		glDeleteBuffers(1, &mBufferId);

		IndexBuffer::destroy_internal();
	}

    void* GLIndexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
        GLenum access = 0;
        if(mIsLocked)
        {
            CM_EXCEPT(InternalErrorException, 
                "Invalid attempt to lock an index buffer that has already been locked");
        }

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);

		if ((options == GBL_WRITE_ONLY) || (options == GBL_WRITE_ONLY_NO_OVERWRITE) || (options == GBL_WRITE_ONLY_DISCARD))
		{
			access = GL_MAP_WRITE_BIT;

			if(options == GBL_WRITE_ONLY_DISCARD)
				access |= GL_MAP_INVALIDATE_BUFFER_BIT;
			else if(options == GBL_WRITE_ONLY_NO_OVERWRITE)
				access |= GL_MAP_UNSYNCHRONIZED_BIT;
		}
		else if (options == GBL_READ_ONLY)
			access = GL_MAP_READ_BIT;
		else
			access = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;

		void* pBuffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, length, access);

		if(pBuffer == 0)
		{
			CM_EXCEPT(InternalErrorException, "Index Buffer: Out of memory");
		}

		void* retPtr = static_cast<void*>(static_cast<unsigned char*>(pBuffer));

		mIsLocked = true;
		return retPtr;
    }

	void GLIndexBuffer::unlockImpl()
    {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);

		if(!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
		{
			CM_EXCEPT(InternalErrorException, "Buffer data corrupted, please reload");
		}

		mIsLocked = false;
    }

    void GLIndexBuffer::readData(UINT32 offset, UINT32 length, 
        void* pDest)
    {
		void* bufferData = lock(offset, length, GBL_READ_ONLY);
		memcpy(pDest, bufferData, length);
		unlock();
    }

    void GLIndexBuffer::writeData(UINT32 offset, UINT32 length, 
		const void* pSource, BufferWriteType writeFlags)
    {
		GpuLockOptions lockOption = GBL_WRITE_ONLY;
		if(writeFlags == BufferWriteType::Discard)
			lockOption = GBL_WRITE_ONLY_DISCARD;
		else if(writeFlags == BufferWriteType::NoOverwrite)
			lockOption = GBL_WRITE_ONLY_NO_OVERWRITE;

		void* bufferData = lock(offset, length, lockOption);
		memcpy(bufferData, pSource, length);
		unlock();
    }
}

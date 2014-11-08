#include "BsGLIndexBuffer.h"
#include "BsGLHardwareBufferManager.h"
#include "BsRenderStats.h"
#include "BsException.h"

namespace BansheeEngine 
{
	GLIndexBufferCore::GLIndexBufferCore(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage)
		:IndexBufferCore(idxType, numIndexes, usage), mZeroLocked(false)
	{  }

	void GLIndexBufferCore::initialize()
	{
		glGenBuffers(1, &mBufferId );

		if (!mBufferId)
		{
			BS_EXCEPT(InternalErrorException, "Cannot create GL index buffer");
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);

		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mSizeInBytes, NULL, 
			GLHardwareBufferCoreManager::getGLUsage(mUsage));

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_IndexBuffer);
		IndexBufferCore::initialize();
	}

	void GLIndexBufferCore::destroy()
	{
		glDeleteBuffers(1, &mBufferId);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_IndexBuffer);
		IndexBufferCore::destroy();
	}

	void* GLIndexBufferCore::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
        GLenum access = 0;
        if(mIsLocked)
        {
            BS_EXCEPT(InternalErrorException, 
                "Invalid attempt to lock an index buffer that has already been locked");
        }

#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_IndexBuffer);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_IndexBuffer);
		}
#endif

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

		void* pBuffer = nullptr;
		
		if (length > 0)
		{
			pBuffer = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, length, access);

			if (pBuffer == nullptr)
			{
				BS_EXCEPT(InternalErrorException, "Index Buffer: Out of memory");
			}

			mZeroLocked = false;
		}
		else
			mZeroLocked = true;

		void* retPtr = static_cast<void*>(static_cast<unsigned char*>(pBuffer));

		mIsLocked = true;
		return retPtr;
    }

	void GLIndexBufferCore::unlockImpl()
    {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mBufferId);

		if (!mZeroLocked)
		{
			if (!glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER))
			{
				BS_EXCEPT(InternalErrorException, "Buffer data corrupted, please reload");
			}
		}

		mIsLocked = false;
    }

	void GLIndexBufferCore::readData(UINT32 offset, UINT32 length,
        void* pDest)
    {
		void* bufferData = lock(offset, length, GBL_READ_ONLY);
		memcpy(pDest, bufferData, length);
		unlock();
    }

    void GLIndexBufferCore::writeData(UINT32 offset, UINT32 length, 
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

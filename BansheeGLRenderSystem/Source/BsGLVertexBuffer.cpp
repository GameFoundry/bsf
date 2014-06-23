#include "BsGLHardwareBufferManager.h"
#include "BsGLVertexBuffer.h"
#include "BsGLVertexArrayObjectManager.h"
#include "BsRenderStats.h"
#include "BsException.h"

namespace BansheeEngine 
{
    GLVertexBuffer::GLVertexBuffer(UINT32 vertexSize, 
        UINT32 numVertices, GpuBufferUsage usage)
        : VertexBuffer(vertexSize, numVertices, usage, false)
    {
    }

    GLVertexBuffer::~GLVertexBuffer()
    {
    }

	void GLVertexBuffer::initialize_internal()
	{
		glGenBuffers(1, &mBufferId);

		if (!mBufferId)
		{
			BS_EXCEPT(InternalErrorException,
				"Cannot create GL vertex buffer");
		}

		glBindBuffer(GL_ARRAY_BUFFER, mBufferId);

		glBufferData(GL_ARRAY_BUFFER, mSizeInBytes, NULL,
			GLHardwareBufferManager::getGLUsage(mUsage));

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_VertexBuffer);
		VertexBuffer::initialize_internal();
	}

	void GLVertexBuffer::destroy_internal()
	{
		glDeleteBuffers(1, &mBufferId);

		while (mVAObjects.size() > 0)
			GLVertexArrayObjectManager::instance().notifyBufferDestroyed(mVAObjects[0]);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_VertexBuffer);
		VertexBuffer::destroy_internal();
	}

	void GLVertexBuffer::registerVAO(const GLVertexArrayObject& vao)
	{
		mVAObjects.push_back(vao);
	}

	void GLVertexBuffer::unregisterVAO(const GLVertexArrayObject& vao)
	{
		auto iterFind = std::find(mVAObjects.begin(), mVAObjects.end(), vao);

		if (iterFind != mVAObjects.end())
			mVAObjects.erase(iterFind);
	}

    void* GLVertexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
    {
        GLenum access = 0;

        if(mIsLocked)
        {
			BS_EXCEPT(InternalErrorException,
                "Invalid attempt to lock a vertex buffer that has already been locked");
        }

#if BS_PROFILING_ENABLED
		if (options == GBL_READ_ONLY || options == GBL_READ_WRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResRead, RenderStatObject_VertexBuffer);
		}

		if (options == GBL_READ_WRITE || options == GBL_WRITE_ONLY || options == GBL_WRITE_ONLY_DISCARD || options == GBL_WRITE_ONLY_NO_OVERWRITE)
		{
			BS_INC_RENDER_STAT_CAT(ResWrite, RenderStatObject_VertexBuffer);
		}
#endif

		glBindBuffer(GL_ARRAY_BUFFER, mBufferId);

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

		void* buffer = glMapBufferRange(GL_ARRAY_BUFFER, offset, length, access);

		if(buffer == nullptr)
		{
			BS_EXCEPT(InternalErrorException, "Cannot map vertex buffer.");
		}

		void* retPtr = static_cast<void*>(static_cast<unsigned char*>(buffer));

		mIsLocked = true;
		return retPtr;
    }

	void GLVertexBuffer::unlockImpl(void)
    {
		glBindBuffer(GL_ARRAY_BUFFER, mBufferId);

		if(!glUnmapBuffer(GL_ARRAY_BUFFER))
		{
			BS_EXCEPT(InternalErrorException, "Buffer data corrupted, please reload.");
		}

        mIsLocked = false;
    }

    void GLVertexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
    {
		void* bufferData = lock(offset, length, GBL_READ_ONLY);
		memcpy(pDest, bufferData, length);
		unlock();
    }

    void GLVertexBuffer::writeData(UINT32 offset, UINT32 length, 
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

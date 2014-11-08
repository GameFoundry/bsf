#pragma once

#include "BsGLPrerequisites.h"
#include "BsIndexBuffer.h"

namespace BansheeEngine 
{ 
	/**
	 * @brief	OpenGL implementation of an index buffer.
	 */
    class BS_RSGL_EXPORT GLIndexBufferCore : public IndexBufferCore
    {
    public:
		GLIndexBufferCore(GpuBufferUsage usage, bool useSystemMemory, const IndexBufferProperties& properties);
		~GLIndexBufferCore() { }

		/**
		 * @copydoc IndexBufferCore::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc IndexBufferCore::writeData
		 */
        void writeData(UINT32 offset, UINT32 length, const void* pSource, 
			BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @brief	Returns internal OpenGL index buffer handle.
		 */
        GLuint getGLBufferId() const { return mBufferId; }

	protected:
		/**
		 * @copydoc IndexBufferCore::initialize
		 */
		void initialize();	
		
		/**
		 * @copydoc IndexBufferCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc IndexBufferCore::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc IndexBufferCore::unlockImpl
		 */
		void unlockImpl();

	private:
		GLuint mBufferId;
		bool mZeroLocked;
    };

	/**
	 * @brief	OpenGL implementation of an index buffer.
	 */
    class BS_RSGL_EXPORT GLIndexBuffer : public IndexBuffer
    {
    public:
		~GLIndexBuffer() { }

	protected:
		friend class GLHardwareBufferManager;

		GLIndexBuffer(IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage); 

		/**
		 * @copydoc	CoreObject::createCore
		 */
		virtual SPtr<CoreObjectCore> createCore() const;
    };
}
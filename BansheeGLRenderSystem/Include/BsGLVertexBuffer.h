#pragma once

#include "BsGLPrerequisites.h"
#include "BsVertexBuffer.h"
#include "BsGLVertexArrayObjectManager.h"

namespace BansheeEngine 
{
	/**
	 * @copydoc	VertexBuffer
	 */
    class BS_RSGL_EXPORT GLVertexBuffer : public VertexBuffer 
    {
    public:
        ~GLVertexBuffer();

		/**
		 * @copydoc	HardwareBuffer::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		* @copydoc	HardwareBuffer::writeData
		*/
        void writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @brief	Returns internal OpenGL buffer ID.
		 */
        GLuint getGLBufferId() const { return mBufferId; }

		/**
		 * @brief	Registers a new VertexArrayObject that uses this vertex buffer.
		 */
		void registerVAO(const GLVertexArrayObject& vao);

		/**
		 * @brief	Unregisters a VAO from this vertex buffer. Does not destroy it.
		 */
		void unregisterVAO(const GLVertexArrayObject& vao);

	protected:
		friend class GLHardwareBufferManager;

		GLVertexBuffer(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage); 

		/**
		* @copydoc VertexBuffer::initialize_internal()
		*/
		void initialize_internal();

		/**
		* @copydoc VertexBuffer::destroy_internal()
		*/
		void destroy_internal();

		/**
		* @copydoc	HardwareBuffer::lockImpl()
		*/
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		* @copydoc	HardwareBuffer::unlockImpl()
		*/
		void unlockImpl();

	private:
		GLuint mBufferId;

		Vector<GLVertexArrayObject> mVAObjects;
    };
}
#pragma once

#include "BsGLPrerequisites.h"
#include "BsVertexBuffer.h"
#include "BsGLVertexArrayObjectManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	OpenGL implementation of a vertex buffer.
	 */
    class BS_RSGL_EXPORT GLVertexBufferCore : public VertexBufferCore
    {
    public:
		GLVertexBufferCore(UINT32 vertexSize, UINT32 numVertices, GpuBufferUsage usage, bool streamOut);
		~GLVertexBufferCore();

		/**
		 * @copydoc	VertexBufferCore::readData
		 */
        void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		* @copydoc	VertexBufferCore::writeData
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
		/**
		 * @copydoc VertexBufferCore::initialize
		 */
		void initialize();

		/**
		 * @copydoc	VertexBufferCore::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc	VertexBufferCore::unlockImpl
		 */
		void unlockImpl();

	private:
		GLuint mBufferId;
		bool mZeroLocked;

		Vector<GLVertexArrayObject> mVAObjects;
    };
}
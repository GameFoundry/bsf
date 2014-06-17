#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of a generic GPU buffer.
	 */
	// TODO - Not implemented, just a dummy class for now
	class BS_RSGL_EXPORT GLGpuBuffer : public GpuBuffer
	{
	public:
        ~GLGpuBuffer();

		/**
		 * @copydoc GpuBuffer::lockImpl
		 */
		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc GpuBuffer::unlockImpl
		 */
		virtual void unlock();

		/**
		 * @copydoc GpuBuffer::readData
		 */
        virtual void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc GpuBuffer::writeData
		 */
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource,
				BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @copydoc GpuBuffer::copyData
		 */
		void copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

	protected:
		friend class GLHardwareBufferManager;

		GLGpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @copydoc GpuBuffer::initialize_internal
		 */
		void initialize_internal();	

		/**
		 * @copydoc GpuBuffer::createView
		 */
		virtual GpuBufferView* createView();

		/**
		 * @copydoc GpuBuffer::destroyView
		 */
		virtual void destroyView(GpuBufferView* view);
	};
}
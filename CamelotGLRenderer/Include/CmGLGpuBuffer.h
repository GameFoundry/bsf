#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuBuffer.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLGpuBuffer : public GpuBuffer
	{
	public:
        ~GLGpuBuffer();

		/**
		 * @copydoc GenericBuffer::lockImpl
		 */
		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc GenericBuffer::unlockImpl
		 */
		virtual void unlock();

		/**
		* @copydoc GenericBuffer::readData
		*/
        virtual void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		* @copydoc GenericBuffer::writeData
		*/
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource,
				bool discardWholeBuffer = false);

		/**
		* @copydoc GenericBuffer::copyData
		*/
		void copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

	protected:
		friend class GLHardwareBufferManager;

		GLGpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		virtual GpuBufferView* createView();
		virtual void destroyView(GpuBufferView* view);

		/**
		 * @copydoc GpuBuffer::initialize_internal()
		 */
		void initialize_internal();	
	};
}
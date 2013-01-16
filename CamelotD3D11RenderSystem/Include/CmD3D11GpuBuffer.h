#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuBuffer.h"
#include "CmCommonEnums.h"

namespace CamelotEngine 
{
	class CM_D3D11_EXPORT D3D11GpuBuffer : public GpuBuffer
    {
    public:
		D3D11GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
        ~D3D11GpuBuffer();

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
		D3D11HardwareBuffer* mBuffer;

		virtual GpuBufferView* createView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite);
		virtual void destroyView(GpuBufferView* view);
    };
}

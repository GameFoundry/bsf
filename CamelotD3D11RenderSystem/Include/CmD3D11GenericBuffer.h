#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGenericBuffer.h"
#include "CmCommonEnums.h"

namespace CamelotEngine 
{
	class CM_D3D11_EXPORT D3D11GenericBuffer : public GenericBuffer
    {
    public:
		D3D11GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
        ~D3D11GenericBuffer();

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
		void copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

	private:
		ID3D11Buffer* mBuffer;

		ID3D11ShaderResourceView* createSRV(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements);
		ID3D11UnorderedAccessView* createUAV(UINT32 firstElement, UINT32 numElements);
    };
}

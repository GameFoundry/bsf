#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGenericBuffer.h"
#include "CmCommonEnums.h"

namespace CamelotEngine 
{
	class CM_D3D11_EXPORT D3D11GenericBuffer : public GenericBuffer
    {
    public:
		D3D11GenericBuffer(GenericBufferType type, GpuBufferUsage usage);
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
    };
}

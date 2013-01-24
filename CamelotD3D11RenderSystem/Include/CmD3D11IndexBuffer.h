#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmIndexBuffer.h"
#include "CmD3D11HardwareBuffer.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(D3D11Device& device, HardwareBufferManager* mgr, IndexType idxType, UINT32 numIndexes, 
			GpuBufferUsage usage, bool useSystemMem);
		~D3D11IndexBuffer();

		/**
		 * @copydoc HardwareBuffer::readData
		 */
		void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc HardwareBuffer::writeData
		 */
		void writeData(UINT32 offset, UINT32 length, const void* pSource,
			bool discardWholeBuffer = false);

		/**
		 * @copydoc HardwareBuffer::copyData
		 */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/**
		 * @brief	Get the D3D-specific index buffer
		 */
		ID3D11Buffer* getD3DIndexBuffer() const { return mBuffer->getD3DBuffer(); }		

	protected:
		/**
		* @copydoc HardwareBuffer::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc HardwareBuffer::unlockImpl
		 */
		void unlockImpl(void);

		void destroy_internal();

		D3D11HardwareBuffer* mBuffer;
	};
}
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsIndexBuffer.h"
#include "BsD3D11HardwareBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of an index buffer.
	 */
	class BS_D3D11_EXPORT D3D11IndexBuffer : public IndexBuffer
	{
	public:
		~D3D11IndexBuffer();

		/**
		 * @copydoc HardwareBuffer::readData
		 */
		void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc HardwareBuffer::writeData
		 */
		void writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @copydoc HardwareBuffer::copyData
		 */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/**
		 * @brief	Gets the internal DX11 index buffer object.
		 */
		ID3D11Buffer* getD3DIndexBuffer() const { return mBuffer->getD3DBuffer(); }		

	protected:
		friend class D3D11HardwareBufferManager;

		/**
		 * @copydoc	IndexBuffer::IndexBuffer.
		 */
		D3D11IndexBuffer(D3D11Device& device, IndexType idxType, UINT32 numIndexes, 
			GpuBufferUsage usage, bool useSystemMem);

		/**
		 * @copydoc HardwareBuffer::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc HardwareBuffer::unlockImpl
		 */
		void unlockImpl(void);

		/**
		 * @copydoc IndexBuffer::initialize_internal()
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc IndexBuffer::destroy_internal()
		 */
		void destroy_internal();

		D3D11HardwareBuffer* mBuffer;
		D3D11Device& mDevice;
	};
}
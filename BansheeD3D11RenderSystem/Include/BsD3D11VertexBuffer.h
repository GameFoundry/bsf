#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsVertexBuffer.h"
#include "BsD3D11HardwareBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of a vertex buffer.
	 */
	class BS_D3D11_EXPORT D3D11VertexBuffer : public VertexBuffer
	{
	public:
		~D3D11VertexBuffer();

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
		 * @brief	Get the D3D-specific index buffer
		 */
		ID3D11Buffer* getD3DVertexBuffer() const { return mBuffer->getD3DBuffer(); }		

	protected: 
		friend class D3D11HardwareBufferManager;

		/**
		 * @copydoc	VertexBuffer::VertexBuffer
		 */
		D3D11VertexBuffer(D3D11Device& device, UINT32 vertexSize, UINT32 numVertices, 
			GpuBufferUsage usage, bool useSystemMem, bool streamOut);

		/**
		* @copydoc HardwareBuffer::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc HardwareBuffer::unlockImpl
		 */
		void unlockImpl(void);

		/**
		 * @copydoc VertexBuffer::initialize_internal()
		 */
		void initialize_internal();	
		
		/**
		 * @copydoc VertexBuffer::destroy_internal()
		 */
		void destroy_internal();

		D3D11HardwareBuffer* mBuffer;
		bool mStreamOut;
		D3D11Device& mDevice;
	};
}
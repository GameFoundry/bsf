#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsIndexBuffer.h"
#include "BsD3D11HardwareBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 11 implementation of an index buffer.
	 */
	class BS_D3D11_EXPORT D3D11IndexBufferCore : public IndexBufferCore
	{
	public:
		D3D11IndexBufferCore(D3D11Device& device, IndexType idxType, UINT32 numIndexes, GpuBufferUsage usage);

		~D3D11IndexBufferCore() { }

		/**
		 * @copydoc IndexBufferCore::readData
		 */
		void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc IndexBufferCore::writeData
		 */
		void writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @copydoc IndexBufferCore::copyData
		 */
		void copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/**
		 * @brief	Gets the internal DX11 index buffer object.
		 */
		ID3D11Buffer* getD3DIndexBuffer() const { return mBuffer->getD3DBuffer(); }		

	protected:
		/**
		 * @copydoc IndexBufferCore::lockImpl
		 */
		void* lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc IndexBufferCore::unlockImpl
		 */
		void unlockImpl();

		/**
		 * @copydoc IndexBufferCore::initialize
		 */
		void initialize();	
		
		/**
		 * @copydoc IndexBufferCore::destroy
		 */
		void destroy();

		D3D11HardwareBuffer* mBuffer;
		D3D11Device& mDevice;
	};
}
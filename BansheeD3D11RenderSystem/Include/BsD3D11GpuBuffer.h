#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuBuffer.h"

namespace BansheeEngine 
{
	/**
	 * @brief	DirectX 11 implementation of a generic GPU buffer.
	 */
	class BS_D3D11_EXPORT D3D11GpuBufferCore : public GpuBufferCore
    {
    public:
		~D3D11GpuBufferCore();

		/**
		 * @copydoc GpuBufferCore::lock
		 */
		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options);

		/**
		 * @copydoc GpuBufferCore::unlock
		 */
		virtual void unlock();

		/**
		 * @copydoc GpuBufferCore::readData
		 */
        virtual void readData(UINT32 offset, UINT32 length, void* pDest);

		/**
		 * @copydoc GpuBufferCore::writeData
		 */
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource,
			BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		 * @copydoc GpuBufferCore::copyData
		 */
		void copyData(GpuBufferCore& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

		/**
		 * @brief	Returns the internal DX11 GPU buffer object.
		 */
		ID3D11Buffer* getDX11Buffer() const;

	protected:
		friend class D3D11HardwareBufferCoreManager;

		/**
		 * @copydoc	GpuBufferCore::GpuBufferCore
		 */
		D3D11GpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage,
			bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @copydoc GpuBufferCore::createView
		 */
		virtual GpuBufferView* createView();

		/**
		 * @copydoc GpuBufferCore::destroyView
		 */
		virtual void destroyView(GpuBufferView* view);

		/**
		 * @copydoc GpuBufferCore::initialize
		 */
		void initialize();	
		
		/**
		 * @copydoc GpuBufferCore::destroy
		 */
		void destroy();

	private:
		D3D11HardwareBuffer* mBuffer;
    };
}

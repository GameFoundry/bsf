#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsGpuBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a generic GPU buffer in DX9. This class
	 *			is just a dummy in order to conform to the interface
	 *			as DX9 supports no such buffers.
	 */
	class BS_D3D9_EXPORT D3D9GpuBufferCore : public GpuBufferCore
	{
	public:
		~D3D9GpuBufferCore();

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

	protected:
		friend class D3D9HardwareBufferCoreManager;

		D3D9GpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage,
			bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @copydoc	GpuBufferCore::createView
		 */
		virtual GpuBufferView* createView();

		/**
		 * @copydoc	GpuBufferCore::destroyView
		 */
		virtual void destroyView(GpuBufferView* view);

		/**
		 * @copydoc GpuBufferCore::initialize
		 */
		void initialize();	
	};
}
//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
	class BS_D3D9_EXPORT D3D9GpuBuffer : public GpuBuffer
	{
	public:
        ~D3D9GpuBuffer();

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
				BufferWriteType writeFlags = BufferWriteType::Normal);

		/**
		* @copydoc GenericBuffer::copyData
		*/
		void copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false);

	protected:
		friend class D3D9HardwareBufferManager;

		D3D9GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @copydoc	GpuBuffer::createView
		 */
		virtual GpuBufferView* createView();

		/**
		 * @copydoc	GpuBuffer::destroyView
		 */
		virtual void destroyView(GpuBufferView* view);

		/**
		 * @copydoc GpuBuffer::initialize_internal()
		 */
		void initialize_internal();	
	};
}
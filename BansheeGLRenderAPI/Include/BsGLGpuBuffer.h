//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of a generic GPU buffer.
	 */
	// TODO - Not implemented, just a dummy class for now
	class BS_RSGL_EXPORT GLGpuBufferCore : public GpuBufferCore
	{
	public:
		~GLGpuBufferCore();

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
		friend class GLHardwareBufferCoreManager;

		GLGpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, 
			GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		/**
		 * @copydoc GpuBufferCore::initialize
		 */
		void initialize();	

		/**
		 * @copydoc GpuBufferCore::createView
		 */
		virtual GpuBufferView* createView();

		/**
		 * @copydoc GpuBufferCore::destroyView
		 */
		virtual void destroyView(GpuBufferView* view);
	};
}
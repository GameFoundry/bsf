//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuBuffer.h"

namespace BansheeEngine
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	OpenGL implementation of a generic GPU buffer. */
	// TODO - Not implemented, just a dummy class for now
	class BS_RSGL_EXPORT GLGpuBufferCore : public GpuBufferCore
	{
	public:
		~GLGpuBufferCore();

		/** @copydoc GpuBufferCore::lock */
		void* lock(UINT32 offset, UINT32 length, GpuLockOptions options) override;

		/** @copydoc GpuBufferCore::unlock */
		void unlock() override;

		/** @copydoc GpuBufferCore::readData */
        void readData(UINT32 offset, UINT32 length, void* pDest) override;

		/** @copydoc GpuBufferCore::writeData */
        void writeData(UINT32 offset, UINT32 length, const void* pSource,
				BufferWriteType writeFlags = BufferWriteType::Normal) override;

		/** @copydoc GpuBufferCore::copyData */
		void copyData(GpuBufferCore& srcBuffer, UINT32 srcOffset,
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false) override;

	protected:
		friend class GLHardwareBufferCoreManager;

		GLGpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, 
			GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		/** @copydoc GpuBufferCore::initialize */
		void initialize() override;

		/** @copydoc GpuBufferCore::createView */
		virtual GpuBufferView* createView() override;

		/** @copydoc GpuBufferCore::destroyView */
		virtual void destroyView(GpuBufferView* view) override;
	};

	/** @} */
}
#pragma once

#include "BsGLPrerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Handles creation of OpenGL specific hardware buffers.
	 */
    class BS_RSGL_EXPORT GLHardwareBufferManager : public HardwareBufferManager
    {
	protected:
		/** @copydoc HardwareBufferManager::createGpuParamBlockBufferImpl */
		GpuParamBlockBufferPtr createGpuParamBlockBufferImpl();

		/**
		 * @copydoc HardwareBufferManager::createGenericBufferImpl
		 */
		GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };

	/**
	 * @brief	Handles creation of OpenGL specific hardware buffers.
	 */
    class BS_RSGL_EXPORT GLHardwareBufferCoreManager : public HardwareBufferCoreManager
    {
    public:
		/**
		 * @brief	Converts engine buffer usage flags into OpenGL specific flags.
		 */
		static GLenum getGLUsage(GpuBufferUsage usage);

		/**
		 * @brief	Converts vertex element type into OpenGL specific type.
		 */
        static GLenum getGLType(VertexElementType type);

	protected:
		/**
		 * @copydoc HardwareBufferCoreManager::createVertexBufferImpl
		 */
        SPtr<VertexBufferCore> createVertexBufferImpl(UINT32 vertexSize, 
            UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferImpl
		 */
        SPtr<IndexBufferCore> createIndexBufferImpl(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);
    };
}
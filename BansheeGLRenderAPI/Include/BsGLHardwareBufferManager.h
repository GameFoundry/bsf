//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine 
{
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
        SPtr<VertexBufferCore> createVertexBufferInternal(UINT32 vertexSize, 
			UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false) override;

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferImpl
		 */
		SPtr<IndexBufferCore> createIndexBufferInternal(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage) override;

		/** @copydoc HardwareBufferCoreManager::createGpuParamBlockBufferInternal */
		SPtr<GpuParamBlockBufferCore> createGpuParamBlockBufferInternal(UINT32 size, GpuParamBlockUsage usage = GPBU_DYNAMIC) override;

		/**
		 * @copydoc HardwareBufferCoreManager::createGenericBufferInternal
		 */
		SPtr<GpuBufferCore> createGpuBufferInternal(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false) override;
    };
}
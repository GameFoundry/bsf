#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmHardwareBufferManager.h"

namespace BansheeEngine {

    /** Implementation of HardwareBufferManager for D3D9. */
    class BS_D3D9_EXPORT D3D9HardwareBufferManager : public HardwareBufferManager
    {
    public:
        D3D9HardwareBufferManager();
        ~D3D9HardwareBufferManager();

	protected:     
		/// Internal method for creates a new vertex declaration, may be overridden by certain rendering APIs
		VertexDeclarationPtr createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements);

		/**
		 * @copydoc HardwareBufferManager::createVertexBufferImpl
		 */
		VertexBufferPtr createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferManager::createIndexBufferImpl
		 */
		IndexBufferPtr createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		/** @copydoc HardwareBufferManager::createGpuParamBlockBufferImpl */
		GpuParamBlockBufferPtr createGpuParamBlockBufferImpl();

		/**
		 * @copydoc HardwareBufferManager::createGenericBufferImpl
		 *
		 * DirectX 9 does not support generic buffers so this method will return a dummy instance.
		 */
		GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };
}
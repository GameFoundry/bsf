#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Manages creation of DX9 hardware buffers.
	 */
    class BS_D3D9_EXPORT D3D9HardwareBufferManager : public HardwareBufferManager
    {
	protected:     
		/**
		 * @copydoc	HardwareBufferManager::createVertexDeclarationImpl
		 */
		VertexDeclarationPtr createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements);

		/** @copydoc HardwareBufferManager::createGpuParamBlockBufferImpl */
		GpuParamBlockBufferPtr createGpuParamBlockBufferImpl();

		/**
		 * @copydoc HardwareBufferManager::createGenericBufferImpl
		 */
		GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };

	/**
	 * @brief	Manages creation of DX9 hardware buffers.
	 */
    class BS_D3D9_EXPORT D3D9HardwareBufferCoreManager : public HardwareBufferCoreManager
    {
	protected:     
		/**
		 * @copydoc HardwareBufferCoreManager::createVertexBufferImpl
		 */
		SPtr<VertexBufferCore> createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferImpl
		 */
		SPtr<IndexBufferCore> createIndexBufferImpl(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);
    };
}
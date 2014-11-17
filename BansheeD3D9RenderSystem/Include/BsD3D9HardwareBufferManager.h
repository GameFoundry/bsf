#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine 
{
	/**
	 * @brief	Manages creation of DX9 hardware buffers.
	 */
    class BS_D3D9_EXPORT D3D9HardwareBufferCoreManager : public HardwareBufferCoreManager
    {
	protected:     
		/**
		 * @copydoc HardwareBufferCoreManager::createVertexBufferInternal
		 */
		SPtr<VertexBufferCore> createVertexBufferInternal(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferInternal
		 */
		SPtr<IndexBufferCore> createIndexBufferInternal(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		/**
		 * @copydoc	HardwareBufferCoreManager::createVertexDeclarationInternal
		 */
		SPtr<VertexDeclarationCore> createVertexDeclarationInternal(const List<VertexElement>& elements);

		/** 
		 * @copydoc HardwareBufferCoreManager::createGpuParamBlockBufferInternal 
		 */
		SPtr<GpuParamBlockBufferCore> createGpuParamBlockBufferInternal(UINT32 size, GpuParamBlockUsage usage = GPBU_DYNAMIC);

		/**
		 * @copydoc HardwareBufferCoreManager::createGenericBufferInternal
		 */
		SPtr<GpuBufferCore> createGpuBufferInternal(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
    };
}
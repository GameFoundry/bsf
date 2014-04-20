#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHardwareBufferManager.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11HardwareBufferManager : public HardwareBufferManager
	{
	public:
		D3D11HardwareBufferManager(D3D11Device& device);
		~D3D11HardwareBufferManager();

	protected:     
		D3D11Device& mDevice;

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
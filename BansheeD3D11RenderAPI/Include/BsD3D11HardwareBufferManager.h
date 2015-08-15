#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 11 hardware buffers.
	 */
	class BS_D3D11_EXPORT D3D11HardwareBufferCoreManager : public HardwareBufferCoreManager
	{
	public:
		D3D11HardwareBufferCoreManager(D3D11Device& device);

	protected:     
		/**
		 * @copydoc HardwareBufferCoreManager::createVertexBufferImpl
		 */
		SPtr<VertexBufferCore> createVertexBufferInternal(UINT32 vertexSize, UINT32 numVerts, 
			GpuBufferUsage usage, bool streamOut = false) override;

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferImpl
		 */
		SPtr<IndexBufferCore> createIndexBufferInternal(IndexType itype, UINT32 numIndexes, 
			GpuBufferUsage usage) override;

		/** 
		 * @copydoc HardwareBufferCoreManager::createGpuParamBlockBufferInternal 
		 */
		SPtr<GpuParamBlockBufferCore> createGpuParamBlockBufferInternal(UINT32 size, 
			GpuParamBlockUsage usage = GPBU_DYNAMIC) override;

		/**
		 * @copydoc HardwareBufferCoreManager::createGpuBufferInternal
		 */
		SPtr<GpuBufferCore> createGpuBufferInternal(UINT32 elementCount, UINT32 elementSize,
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false) override;

		D3D11Device& mDevice;
	};
}
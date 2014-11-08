#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles creation of DirectX 11 hardware buffers.
	 */
	class BS_D3D11_EXPORT D3D11HardwareBufferManager : public HardwareBufferManager
	{
	public:
		D3D11HardwareBufferManager(D3D11Device& device);

	protected:     
		/** 
		 * @copydoc HardwareBufferManager::createGpuParamBlockBufferImpl 
		 */
		GpuParamBlockBufferPtr createGpuParamBlockBufferImpl();

		/**
		 * @copydoc HardwareBufferManager::createGenericBufferImpl
		 */
		GpuBufferPtr createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

		D3D11Device& mDevice;
	};

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
		SPtr<VertexBufferCore> createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferCoreManager::createIndexBufferImpl
		 */
		SPtr<IndexBufferCore> createIndexBufferImpl(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		D3D11Device& mDevice;
	};
}
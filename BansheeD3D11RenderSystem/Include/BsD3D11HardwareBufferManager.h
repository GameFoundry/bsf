//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
		~D3D11HardwareBufferManager();

	protected:     
		/**
		 * @copydoc HardwareBufferManager::createVertexBufferImpl
		 */
		VertexBufferPtr createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);

		/**
		 * @copydoc HardwareBufferManager::createIndexBufferImpl
		 */
		IndexBufferPtr createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

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
}
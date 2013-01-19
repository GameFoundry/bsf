#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHardwareBufferManager.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11HardwareBufferManager : public HardwareBufferManager
	{
	public:
		D3D11HardwareBufferManager(D3D11Device& device);
		~D3D11HardwareBufferManager();

		/**
		 * @brief	Creates a hardware vertex buffer.
		 */
		VertexBufferPtr createVertexBuffer(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut = false);
		
		/**
		 * @brief	Creates a hardware index buffer.
		 */
		IndexBufferPtr createIndexBuffer(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage);

		/** @copydoc HardwareBufferManagerInterface::createGpuParamBlockBuffer */
		GpuParamBlockBufferPtr createGpuParamBlockBuffer(const GpuParamBlockDesc& paramDesc);

		/**
		 * @copydoc HardwareBufferManagerInterface::createGpuBuffer
		 *
		 * Some limitations are imposed on the combinations of parameters:
		 *  - APPENDCONSUME buffer type can only be used with randomGpuWrite enabled  
		 *  - Counter can only be used with STRUCTURED buffer type
		 */
		GpuBufferPtr createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
			GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);

	protected:     
		/// Internal method for creates a new vertex declaration, may be overridden by certain rendering APIs
		VertexDeclarationPtr createVertexDeclarationImpl(void);
		/// Internal method for destroys a vertex declaration, may be overridden by certain rendering APIs
		void destroyVertexDeclarationImpl(VertexDeclaration* decl);

		D3D11Device& mDevice;
	};
}
#include "CmD3D9HardwareBufferManager.h"
#include "CmD3D9VertexBuffer.h"
#include "CmD3D9IndexBuffer.h"
#include "CmD3D9VertexDeclaration.h"
#include "CmD3D9GpuBuffer.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmException.h"

namespace BansheeEngine 
{
    D3D9HardwareBufferManager::D3D9HardwareBufferManager()       
    {
    }

    D3D9HardwareBufferManager::~D3D9HardwareBufferManager()
    {
    }

    VertexBufferPtr D3D9HardwareBufferManager::createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
    {
		assert (numVerts > 0);

        D3D9VertexBuffer* buffer = new (cm_alloc<D3D9VertexBuffer, PoolAlloc>()) D3D9VertexBuffer(vertexSize, numVerts, usage, false);
		return cm_core_ptr<D3D9VertexBuffer, PoolAlloc>(buffer);
    }

	IndexBufferPtr D3D9HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
    {
		assert (numIndexes > 0);

		D3D9IndexBuffer* buffer = new (cm_alloc<D3D9IndexBuffer, PoolAlloc>()) D3D9IndexBuffer(itype, numIndexes, usage, false);
		return cm_core_ptr<D3D9IndexBuffer, PoolAlloc>(buffer);
    }

	GpuParamBlockBufferPtr D3D9HardwareBufferManager::createGpuParamBlockBufferImpl()
	{
		GpuParamBlockBuffer* paramBlockBuffer = new (cm_alloc<GenericGpuParamBlockBuffer, PoolAlloc>()) GenericGpuParamBlockBuffer();
		return cm_core_ptr<GpuParamBlockBuffer, PoolAlloc>(paramBlockBuffer);
	}

	GpuBufferPtr D3D9HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D9GpuBuffer* buffer = new (cm_alloc<D3D9GpuBuffer, PoolAlloc>()) D3D9GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
		return cm_core_ptr<D3D9GpuBuffer, PoolAlloc>(buffer);
	}

	VertexDeclarationPtr D3D9HardwareBufferManager::createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements)
    {
		D3D9VertexDeclaration* decl = new (cm_alloc<D3D9VertexDeclaration, PoolAlloc>()) D3D9VertexDeclaration(elements);
		return cm_core_ptr<D3D9VertexDeclaration, PoolAlloc>(decl);
    }
}

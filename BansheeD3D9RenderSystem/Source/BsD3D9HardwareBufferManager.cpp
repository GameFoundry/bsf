#include "BsD3D9HardwareBufferManager.h"
#include "BsD3D9VertexBuffer.h"
#include "BsD3D9IndexBuffer.h"
#include "BsD3D9VertexDeclaration.h"
#include "BsD3D9GpuBuffer.h"
#include "BsGpuParamBlockBuffer.h"
#include "BsException.h"

namespace BansheeEngine 
{
	GpuParamBlockBufferPtr D3D9HardwareBufferManager::createGpuParamBlockBufferImpl()
	{
		GpuParamBlockBuffer* paramBlockBuffer = new (bs_alloc<GenericGpuParamBlockBuffer, PoolAlloc>()) GenericGpuParamBlockBuffer();
		return bs_core_ptr<GpuParamBlockBuffer, PoolAlloc>(paramBlockBuffer);
	}

	GpuBufferPtr D3D9HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D9GpuBuffer* buffer = new (bs_alloc<D3D9GpuBuffer, PoolAlloc>()) D3D9GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
		return bs_core_ptr<D3D9GpuBuffer, PoolAlloc>(buffer);
	}

	VertexDeclarationPtr D3D9HardwareBufferManager::createVertexDeclarationImpl(const VertexDeclaration::VertexElementList& elements)
    {
		D3D9VertexDeclaration* decl = new (bs_alloc<D3D9VertexDeclaration, PoolAlloc>()) D3D9VertexDeclaration(elements);
		return bs_core_ptr<D3D9VertexDeclaration, PoolAlloc>(decl);
    }

	SPtr<VertexBufferCore> D3D9HardwareBufferCoreManager::createVertexBufferImpl(UINT32 vertexSize, UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert(numVerts > 0);

		return bs_shared_ptr<D3D9VertexBufferCore>(vertexSize, numVerts, usage, streamOut);
	}

	SPtr<IndexBufferCore> D3D9HardwareBufferCoreManager::createIndexBufferImpl(IndexType itype, UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert(numIndexes > 0);

		return bs_shared_ptr<D3D9IndexBufferCore>(itype, numIndexes, usage);
	}
}

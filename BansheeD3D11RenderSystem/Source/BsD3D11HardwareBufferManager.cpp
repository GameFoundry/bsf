#include "BsD3D11HardwareBufferManager.h"
#include "BsD3D11VertexBuffer.h"
#include "BsD3D11IndexBuffer.h"
#include "BsD3D11GpuBuffer.h"
#include "BsD3D11GpuParamBlockBuffer.h"
#include "BsGpuParamDesc.h"

namespace BansheeEngine
{
	D3D11HardwareBufferManager::D3D11HardwareBufferManager(D3D11Device& device)
		: mDevice(device)
	{ }

	D3D11HardwareBufferManager::~D3D11HardwareBufferManager()
	{

	}

	VertexBufferPtr D3D11HardwareBufferManager::createVertexBufferImpl(UINT32 vertexSize, 
		UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		D3D11VertexBuffer* buffer = new (bs_alloc<D3D11VertexBuffer, PoolAlloc>()) D3D11VertexBuffer(mDevice, vertexSize, numVerts, usage, false, streamOut);

		return bs_core_ptr<D3D11VertexBuffer, PoolAlloc>(buffer);
	}

	IndexBufferPtr D3D11HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, 
		UINT32 numIndexes, GpuBufferUsage usage)
	{
		D3D11IndexBuffer* buffer = new (bs_alloc<D3D11IndexBuffer, PoolAlloc>()) D3D11IndexBuffer(mDevice, itype, numIndexes, usage, false);

		return bs_core_ptr<D3D11IndexBuffer, PoolAlloc>(buffer);
	}

	GpuParamBlockBufferPtr D3D11HardwareBufferManager::createGpuParamBlockBufferImpl()
	{
		D3D11GpuParamBlockBuffer* paramBlockBuffer = new (bs_alloc<D3D11GpuParamBlockBuffer, PoolAlloc>()) D3D11GpuParamBlockBuffer();

		return bs_core_ptr<D3D11GpuParamBlockBuffer, PoolAlloc>(paramBlockBuffer);
	}

	GpuBufferPtr D3D11HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D11GpuBuffer* buffer = new (bs_alloc<D3D11GpuBuffer, PoolAlloc>()) D3D11GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);

		return bs_core_ptr<D3D11GpuBuffer, PoolAlloc>(buffer);
	}
}
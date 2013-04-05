#include "CmD3D11HardwareBufferManager.h"
#include "CmD3D11VertexBuffer.h"
#include "CmD3D11IndexBuffer.h"
#include "CmD3D11GpuBuffer.h"
#include "CmD3D11GpuParamBlock.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine
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
		D3D11VertexBuffer* buffer = CM_NEW(D3D11VertexBuffer, PoolAlloc) D3D11VertexBuffer(mDevice, this, vertexSize, numVerts, usage, false, streamOut);

		return VertexBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D11VertexBuffer, PoolAlloc>);
	}

	IndexBufferPtr D3D11HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, 
		UINT32 numIndexes, GpuBufferUsage usage)
	{
		D3D11IndexBuffer* buffer = CM_NEW(D3D11IndexBuffer, PoolAlloc) D3D11IndexBuffer(mDevice, this, itype, numIndexes, usage, false);

		return IndexBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D11IndexBuffer, PoolAlloc>);
	}

	GpuParamBlockPtr D3D11HardwareBufferManager::createGpuParamBlockImpl()
	{
		D3D11GpuParamBlock* paramBlock = CM_NEW(D3D11GpuParamBlock, PoolAlloc) D3D11GpuParamBlock();

		return GpuParamBlockPtr(paramBlock, &CoreObject::_deleteDelayed<D3D11GpuParamBlock, PoolAlloc>);
	}

	GpuBufferPtr D3D11HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		D3D11GpuBuffer* buffer = CM_NEW(D3D11GpuBuffer, PoolAlloc) D3D11GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);

		return GpuBufferPtr(buffer, &CoreObject::_deleteDelayed<D3D11GpuBuffer, PoolAlloc>);
	}
}
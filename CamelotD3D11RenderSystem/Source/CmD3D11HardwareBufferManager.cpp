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
		return VertexBufferPtr(new D3D11VertexBuffer(mDevice, this, vertexSize, numVerts, usage, false, streamOut));
	}

	IndexBufferPtr D3D11HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, 
		UINT32 numIndexes, GpuBufferUsage usage)
	{
		return IndexBufferPtr(new D3D11IndexBuffer(mDevice, this, itype, numIndexes, usage, false));
	}

	GpuParamBlockPtr D3D11HardwareBufferManager::createGpuParamBlockImpl(const GpuParamBlockDesc& blockDesc, GpuParamBlockUsage usage)
	{
		return GpuParamBlockPtr(new D3D11GpuParamBlock(blockDesc, usage));
	}

	GpuBufferPtr D3D11HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		return GpuBufferPtr(new D3D11GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter));
	}
}
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

	VertexBuffer* D3D11HardwareBufferManager::createVertexBufferImpl(UINT32 vertexSize, 
		UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		return new D3D11VertexBuffer(mDevice, this, vertexSize, numVerts, usage, false, streamOut);
	}

	IndexBuffer* D3D11HardwareBufferManager::createIndexBufferImpl(IndexBuffer::IndexType itype, 
		UINT32 numIndexes, GpuBufferUsage usage)
	{
		return new D3D11IndexBuffer(mDevice, this, itype, numIndexes, usage, false);
	}

	GpuParamBlock* D3D11HardwareBufferManager::createGpuParamBlockImpl(const GpuParamBlockDesc& blockDesc, GpuParamBlockUsage usage)
	{
		return new D3D11GpuParamBlock(blockDesc, usage);
	}

	GpuBuffer* D3D11HardwareBufferManager::createGpuBufferImpl(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		return new D3D11GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter);
	}
}
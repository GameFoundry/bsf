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

	VertexBufferPtr D3D11HardwareBufferManager::createVertexBuffer(UINT32 vertexSize, 
		UINT32 numVerts, GpuBufferUsage usage, bool streamOut)
	{
		assert (numVerts > 0);
		D3D11VertexBuffer* vbuf = new D3D11VertexBuffer(mDevice,
			this, vertexSize, numVerts, usage, false, streamOut);
		{
			mVertexBuffers.insert(vbuf);
		}

		return VertexBufferPtr(vbuf);
	}

	IndexBufferPtr D3D11HardwareBufferManager::createIndexBuffer(IndexBuffer::IndexType itype, 
		UINT32 numIndexes, GpuBufferUsage usage)
	{
		assert (numIndexes > 0);

		D3D11IndexBuffer* idx = new D3D11IndexBuffer(mDevice,
			this, itype, numIndexes, usage, false);
		{

				mIndexBuffers.insert(idx);
		}

		return IndexBufferPtr(idx);
	}

	GpuParamBlockPtr D3D11HardwareBufferManager::createGpuParamBlock(const GpuParamBlockDesc& blockDesc)
	{
		return GpuParamBlockPtr(new D3D11GpuParamBlock(blockDesc));
	}

	GenericBufferPtr D3D11HardwareBufferManager::createGpuBuffer(UINT32 elementCount, UINT32 elementSize, 
		GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
	{
		return GenericBufferPtr(new D3D11GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter));
	}

	VertexDeclarationPtr D3D11HardwareBufferManager::createVertexDeclarationImpl(void)
	{
		return VertexDeclarationPtr(new VertexDeclaration());
	}

	void D3D11HardwareBufferManager::destroyVertexDeclarationImpl(VertexDeclaration* decl)
	{
		delete decl;
	}
}
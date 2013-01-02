#include "CmD3D11HardwareBufferManager.h"
#include "CmD3D11HardwareVertexBuffer.h"
#include "CmD3D11HardwareIndexBuffer.h"
#include "CmD3D11HardwareConstantBuffer.h"
#include "CmD3D11VertexDeclaration.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine
{
	D3D11HardwareBufferManagerBase::D3D11HardwareBufferManagerBase(D3D11Device& device)
		: mDevice(device)
	{ }

	D3D11HardwareBufferManagerBase::~D3D11HardwareBufferManagerBase()
	{
		destroyAllBindings();
	}

	HardwareVertexBufferPtr D3D11HardwareBufferManagerBase::createVertexBuffer(UINT32 vertexSize, 
		UINT32 numVerts, HardwareBuffer::Usage usage, bool streamOut)
	{
		assert (numVerts > 0);
		D3D11HardwareVertexBuffer* vbuf = new D3D11HardwareVertexBuffer(mDevice,
			this, vertexSize, numVerts, usage, false, streamOut);
		{
			mVertexBuffers.insert(vbuf);
		}

		return HardwareVertexBufferPtr(vbuf);
	}

	HardwareIndexBufferPtr D3D11HardwareBufferManagerBase::createIndexBuffer(HardwareIndexBuffer::IndexType itype, 
		UINT32 numIndexes, HardwareBuffer::Usage usage)
	{
		assert (numIndexes > 0);

		D3D11HardwareIndexBuffer* idx = new D3D11HardwareIndexBuffer(mDevice,
			this, itype, numIndexes, usage, false);
		{

				mIndexBuffers.insert(idx);
		}

		return HardwareIndexBufferPtr(idx);
	}

	GpuParamBlockPtr D3D11HardwareBufferManagerBase::createGpuParamBlock(const GpuParamBlockDesc& blockDesc)
	{
		return GpuParamBlockPtr(new GpuParamBlock(blockDesc));
	}

	VertexDeclarationPtr D3D11HardwareBufferManagerBase::createVertexDeclarationImpl(void)
	{
		return VertexDeclarationPtr(new D3D11VertexDeclaration());
	}

	void D3D11HardwareBufferManagerBase::destroyVertexDeclarationImpl(VertexDeclaration* decl)
	{
		delete decl;
	}
}
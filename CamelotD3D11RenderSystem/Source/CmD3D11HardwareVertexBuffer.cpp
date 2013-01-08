#include "CmD3D11HardwareVertexBuffer.h"

namespace CamelotEngine
{
	D3D11HardwareVertexBuffer::D3D11HardwareVertexBuffer(D3D11Device& device, HardwareBufferManagerBase* mgr, UINT32 vertexSize, UINT32 numVertices, 
		GpuBufferUsage usage, bool useSystemMem, bool streamOut)
		:HardwareVertexBuffer(mgr, vertexSize, numVertices, usage, useSystemMem)
	{
		mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::BT_VERTEX, usage, 1, mSizeInBytes, device, useSystemMem, streamOut);
	}

	D3D11HardwareVertexBuffer::~D3D11HardwareVertexBuffer()
	{
		delete mBuffer;
	}

	void* D3D11HardwareVertexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11HardwareVertexBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11HardwareVertexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11HardwareVertexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11HardwareVertexBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}
}
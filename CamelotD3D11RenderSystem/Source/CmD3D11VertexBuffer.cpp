#include "CmD3D11VertexBuffer.h"

namespace CamelotEngine
{
	D3D11VertexBuffer::D3D11VertexBuffer(D3D11Device& device, HardwareBufferManager* mgr, UINT32 vertexSize, UINT32 numVertices, 
		GpuBufferUsage usage, bool useSystemMem, bool streamOut)
		:VertexBuffer(mgr, vertexSize, numVertices, usage, useSystemMem), mDevice(device), mStreamOut(streamOut)
	{ }

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{ }

	void* D3D11VertexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11VertexBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11VertexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11VertexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11VertexBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}

	void D3D11VertexBuffer::initialize_internal()
	{
		mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::BT_VERTEX, 
			mUsage, 1, mSizeInBytes, mDevice, mSystemMemory, mStreamOut);

		VertexBuffer::initialize_internal();
	}

	void D3D11VertexBuffer::destroy_internal()
	{
		if(mBuffer != nullptr)
			delete mBuffer;

		VertexBuffer::destroy_internal();
	}
}
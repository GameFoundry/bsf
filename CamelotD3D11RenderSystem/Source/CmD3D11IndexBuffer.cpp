#include "CmD3D11IndexBuffer.h"

namespace CamelotEngine
{
	D3D11IndexBuffer::D3D11IndexBuffer(D3D11Device& device, HardwareBufferManager* mgr, IndexType idxType, UINT32 numIndexes, 
		GpuBufferUsage usage, bool useSystemMem)
		:IndexBuffer(mgr, idxType, numIndexes, usage, useSystemMem)
	{
		mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::BT_INDEX, usage, 1, mSizeInBytes, device, useSystemMem);
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		delete mBuffer;
	}

	void* D3D11IndexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11IndexBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11IndexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11IndexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11IndexBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}
}
#include "CmD3D11HardwareIndexBuffer.h"

namespace CamelotEngine
{
	D3D11HardwareIndexBuffer::D3D11HardwareIndexBuffer(D3D11Device& device, HardwareBufferManagerBase* mgr, IndexType idxType, UINT32 numIndexes, 
		GpuBufferUsage usage, bool useSystemMem)
		:HardwareIndexBuffer(mgr, idxType, numIndexes, usage, useSystemMem)
	{
		mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::BT_INDEX, usage, 1, mSizeInBytes, device, useSystemMem);
	}

	D3D11HardwareIndexBuffer::~D3D11HardwareIndexBuffer()
	{
		delete mBuffer;
	}

	void* D3D11HardwareIndexBuffer::lockImpl(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11HardwareIndexBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11HardwareIndexBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11HardwareIndexBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11HardwareIndexBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}
}
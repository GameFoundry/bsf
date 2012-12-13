#include "CmD3D11HardwareConstantBuffer.h"

namespace CamelotEngine
{
	D3D11HardwareConstantBuffer::D3D11HardwareConstantBuffer(D3D11Device& device, HardwareBufferManagerBase* mgr, UINT32 sizeBytes, 
		HardwareBuffer::Usage usage, bool useSystemMem)
		:HardwareConstantBuffer(mgr, sizeBytes, usage, useSystemMem)
	{
		mBuffer = new D3D11HardwareBuffer(D3D11HardwareBuffer::CONSTANT_BUFFER, mSizeInBytes, usage, device, useSystemMem, false);
	}

	D3D11HardwareConstantBuffer::~D3D11HardwareConstantBuffer()
	{
		delete mBuffer;
	}

	void* D3D11HardwareConstantBuffer::lockImpl(UINT32 offset, UINT32 length, LockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11HardwareConstantBuffer::unlockImpl()
	{
		mBuffer->unlock();
	}

	void D3D11HardwareConstantBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11HardwareConstantBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11HardwareConstantBuffer::copyData(HardwareBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		mBuffer->copyData(srcBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}
}
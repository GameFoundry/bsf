#include "CmD3D11IndexBuffer.h"
#include "CmD3D11Device.h"

namespace CamelotFramework
{
	D3D11IndexBuffer::D3D11IndexBuffer(D3D11Device& device, HardwareBufferManager* mgr, IndexType idxType, UINT32 numIndexes, 
		GpuBufferUsage usage, bool useSystemMem)
		:IndexBuffer(mgr, idxType, numIndexes, usage, useSystemMem), mDevice(device), mBuffer(nullptr)
	{

	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
		
	}

	void D3D11IndexBuffer::initialize_internal()
	{
		mBuffer = cm_new<D3D11HardwareBuffer, PoolAlloc>(D3D11HardwareBuffer::BT_INDEX, mUsage, 1, mSizeInBytes, std::ref(mDevice), mSystemMemory);

		IndexBuffer::initialize_internal();
	}

	void D3D11IndexBuffer::destroy_internal()
	{
		if(mBuffer != nullptr)
			cm_delete<PoolAlloc>(mBuffer) ;

		IndexBuffer::destroy_internal();
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
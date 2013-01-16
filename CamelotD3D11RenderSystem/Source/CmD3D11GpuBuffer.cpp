#include "CmD3D11GpuBuffer.h"
#include "CmD3D11GpuBufferView.h"
#include "CmD3D11RenderSystem.h"
#include "CmD3D11HardwareBuffer.h"
#include "CmD3D11Device.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11GpuBuffer::D3D11GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter), mBuffer(nullptr)
	{
		D3D11HardwareBuffer::BufferType bufferType;
		D3D11RenderSystem* d3d11rs = static_cast<D3D11RenderSystem*>(D3D11RenderSystem::instancePtr());

		switch(type)
		{
		case GBT_STRUCTURED:
			bufferType = D3D11HardwareBuffer::BT_STRUCTURED;
			break;
		case GBT_RAW:
			bufferType = D3D11HardwareBuffer::BT_RAW;
			break;
		case GBT_INDIRECTARGUMENT:
			bufferType = D3D11HardwareBuffer::BT_INDIRECTARGUMENT;
			break;
		case GBT_APPENDCONSUME:
			bufferType = D3D11HardwareBuffer::BT_APPENDCONSUME;
			break;
		}

		mBuffer = new D3D11HardwareBuffer(bufferType, usage, elementCount, elementSize, 
			d3d11rs->getPrimaryDevice(), false, false, randomGpuWrite, useCounter);
	}

	D3D11GpuBuffer::~D3D11GpuBuffer()
	{
		delete mBuffer;

		clearBufferViews();
	}

	void* D3D11GpuBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return mBuffer->lock(offset, length, options);
	}

	void D3D11GpuBuffer::unlock()
	{
		mBuffer->unlock();
	}

	void D3D11GpuBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
		mBuffer->readData(offset, length, pDest);
	}

	void D3D11GpuBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
		mBuffer->writeData(offset, length, pSource, discardWholeBuffer);
	}

	void D3D11GpuBuffer::copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
		D3D11GpuBuffer* d3d11SrcBuffer = static_cast<D3D11GpuBuffer*>(&srcBuffer);

		mBuffer->copyData(*d3d11SrcBuffer->mBuffer, srcOffset, dstOffset, length, discardWholeBuffer);
	}

	GpuBufferView* D3D11GpuBuffer::createView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
	{
		return new D3D11GpuBufferView(mBuffer->getD3DBuffer(), mType, firstElement, elementWidth, numElements, randomGpuWrite, mUseCounter);
	}

	void D3D11GpuBuffer::destroyView(GpuBufferView* view)
	{
		delete view;
	}
}
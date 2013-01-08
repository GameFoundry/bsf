#include "CmD3D9GenericBuffer.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	D3D9GenericBuffer::D3D9GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GenericBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
		LOGWRN("Generic buffers are not supported in DirectX 9. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");
	}

	D3D9GenericBuffer::~D3D9GenericBuffer()
	{
		clearBufferViews();
	}

	void* D3D9GenericBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void D3D9GenericBuffer::unlock()
	{
	}

	void D3D9GenericBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
	}

	void D3D9GenericBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
	}

	void D3D9GenericBuffer::copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GenericBufferView* D3D9GenericBuffer::createView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
	{
		return nullptr;
	}

	void D3D9GenericBuffer::destroyView(GenericBufferView* view)
	{
	}
}
#include "CmD3D9GpuBuffer.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	D3D9GpuBuffer::D3D9GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
	}

	D3D9GpuBuffer::~D3D9GpuBuffer()
	{ }

	void D3D9GpuBuffer::initialize_internal()
	{
		LOGWRN("Generic buffers are not supported in DirectX 9. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");

		GpuBuffer::initialize_internal();
	}

	void* D3D9GpuBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void D3D9GpuBuffer::unlock()
	{
	}

	void D3D9GpuBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
	}

	void D3D9GpuBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
	}

	void D3D9GpuBuffer::copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GpuBufferView* D3D9GpuBuffer::createView()
	{
		return nullptr;
	}

	void D3D9GpuBuffer::destroyView(GpuBufferView* view)
	{
	}
}
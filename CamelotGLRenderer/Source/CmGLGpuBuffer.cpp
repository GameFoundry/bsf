#include "CmGLGpuBuffer.h"
#include "CmDebug.h"

namespace CamelotFramework
{
	GLGpuBuffer::GLGpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GpuBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
	}

	GLGpuBuffer::~GLGpuBuffer()
	{
	}

	void GLGpuBuffer::initialize_internal()
	{
		LOGWRN("Generic buffers are not supported in OpenGL. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");

		GpuBuffer::initialize_internal();
	}

	void* GLGpuBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void GLGpuBuffer::unlock()
	{
	}

	void GLGpuBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
	}

	void GLGpuBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, BufferWriteType writeFlags)
	{
	}

	void GLGpuBuffer::copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GpuBufferView* GLGpuBuffer::createView()
	{
		return nullptr;
	}

	void GLGpuBuffer::destroyView(GpuBufferView* view)
	{
	}
}
#include "CmGLGenericBuffer.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	GLGenericBuffer::GLGenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter) 
		: GenericBuffer(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
		LOGWRN("Generic buffers are not supported in OpenGL. Creating a dummy buffer. All operations on it will either be no-op or return a nullptr.");
	}

	GLGenericBuffer::~GLGenericBuffer()
	{
		clearBufferViews();
	}

	void* GLGenericBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void GLGenericBuffer::unlock()
	{
	}

	void GLGenericBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{
	}

	void GLGenericBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{
	}

	void GLGenericBuffer::copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{
	}

	GenericBufferView* GLGenericBuffer::createView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
	{
		return nullptr;
	}

	void GLGenericBuffer::destroyView(GenericBufferView* view)
	{
	}
}
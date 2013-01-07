#include "CmD3D11GenericBuffer.h"

namespace CamelotEngine
{
	D3D11GenericBuffer::D3D11GenericBuffer(GenericBufferType type, GpuBufferUsage usage) 
		: GenericBuffer(type, usage)
	{  }

	D3D11GenericBuffer::~D3D11GenericBuffer() 
	{ }

	void* D3D11GenericBuffer::lock(UINT32 offset, UINT32 length, GpuLockOptions options)
	{
		return nullptr;
	}

	void D3D11GenericBuffer::unlock()
	{

	}

	void D3D11GenericBuffer::readData(UINT32 offset, UINT32 length, void* pDest)
	{

	}

	void D3D11GenericBuffer::writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer)
	{

	}

	void D3D11GenericBuffer::copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
		UINT32 dstOffset, UINT32 length, bool discardWholeBuffer)
	{

	}
}
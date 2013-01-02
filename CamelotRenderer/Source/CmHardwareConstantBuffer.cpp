#include "CmHardwareConstantBuffer.h"
#include "CmHardwareBufferManager.h"

namespace CamelotEngine
{
	HardwareConstantBuffer::HardwareConstantBuffer(HardwareBufferManagerBase* mgr, UINT32 sizeBytes, HardwareBuffer::Usage usage, bool useSystemMemory)
		: HardwareBuffer(usage, useSystemMemory)
	{
		// Calculate the size of the vertices
		mSizeInBytes = sizeBytes;
	}

	HardwareConstantBuffer::~HardwareConstantBuffer()
	{

	}
}
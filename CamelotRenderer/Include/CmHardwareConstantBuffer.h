#pragma once

#include "CmPrerequisites.h"
#include "CmHardwareBuffer.h"

namespace CamelotEngine
{
	class CM_EXPORT HardwareConstantBuffer : public HardwareBuffer
	{
	protected:
		HardwareBufferManagerBase* mMgr;

	public:
		/// Should be called by HardwareBufferManager
		HardwareConstantBuffer(HardwareBufferManagerBase* mgr, UINT32 sizeBytes, HardwareBuffer::Usage usage, bool useSystemMemory);
		~HardwareConstantBuffer();

		/// Return the manager of this buffer, if any
		HardwareBufferManagerBase* getManager() const { return mMgr; }
	};
}
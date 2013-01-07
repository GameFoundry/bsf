#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"

namespace CamelotEngine 
{
	enum GenericBufferType
	{
		GBT_STRUCTURED,
		GBT_RAW,
		GBT_INDIRECTARGUMENT,
		GBT_APPENDCONSUME
	};

	class CM_EXPORT GenericBuffer
    {
    public:
        GenericBuffer(GenericBufferType type, GpuBufferUsage usage)
			:mType(type), mUsage(usage)
        {  }
        virtual ~GenericBuffer() {}


		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;
		virtual void unlock() = 0;

        virtual void readData(UINT32 offset, UINT32 length, void* pDest) = 0;
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer = false) = 0;

		virtual void copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false) = 0;

	private:
		GenericBufferType mType;
		GpuBufferUsage mUsage;
    };
}

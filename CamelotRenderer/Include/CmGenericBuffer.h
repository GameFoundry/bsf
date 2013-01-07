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
        GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false)
			:mElementCount(elementCount), mElementSize(elementSize), mType(type), mUsage(usage), mRandomGpuWrite(randomGpuWrite), mUseCounter(useCounter)
        {  }
        virtual ~GenericBuffer() {}

		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;
		virtual void unlock() = 0;

        virtual void readData(UINT32 offset, UINT32 length, void* pDest) = 0;
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer = false) = 0;

		virtual void copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false) = 0;

	protected:
		GenericBufferType mType;
		GpuBufferUsage mUsage;
		bool mRandomGpuWrite;
		bool mUseCounter;
		UINT32 mElementCount;
		UINT32 mElementSize;
    };
}

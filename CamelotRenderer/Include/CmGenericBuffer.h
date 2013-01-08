#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmGenericBufferView.h"

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
        GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
        virtual ~GenericBuffer();

		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;
		virtual void unlock() = 0;

        virtual void readData(UINT32 offset, UINT32 length, void* pDest) = 0;
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer = false) = 0;

		virtual void copyData(GenericBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false) = 0;

		GenericBufferView* requestView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite);
		void releaseView(GenericBufferView* view);

	protected:
		GenericBufferType mType;
		GpuBufferUsage mUsage;
		bool mRandomGpuWrite;
		bool mUseCounter;
		UINT32 mElementCount;
		UINT32 mElementSize;

		virtual GenericBufferView* createView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite) = 0;
		virtual void destroyView(GenericBufferView* view) = 0;
		void clearBufferViews();

		struct GenericBufferReference
		{
			GenericBufferReference(GenericBufferView* _view)
				:view(_view), refCount(0)
			{ }

			GenericBufferView* view;
			UINT32 refCount;
		};

		std::unordered_map<GenericBufferView::Key, GenericBufferReference*, GenericBufferView::HashFunction, GenericBufferView::EqualFunction> mBufferViews;
    };
}

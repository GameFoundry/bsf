#pragma once

#include "CmPrerequisites.h"
#include "CmCommonEnums.h"
#include "CmGpuBufferView.h"
#include "CmCoreGpuObject.h"

namespace CamelotEngine 
{
	class CM_EXPORT GpuBuffer : public CoreGpuObject
    {
    public:
        GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite = false, bool useCounter = false);
        virtual ~GpuBuffer();

		virtual void* lock(UINT32 offset, UINT32 length, GpuLockOptions options) = 0;
		virtual void unlock() = 0;

        virtual void readData(UINT32 offset, UINT32 length, void* pDest) = 0;
        virtual void writeData(UINT32 offset, UINT32 length, const void* pSource, bool discardWholeBuffer = false) = 0;

		virtual void copyData(GpuBuffer& srcBuffer, UINT32 srcOffset, 
			UINT32 dstOffset, UINT32 length, bool discardWholeBuffer = false) = 0;

		static GpuBufferView* requestView(GpuBufferPtr buffer, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool useCounter, GpuViewUsage usage);
		static void releaseView(GpuBufferView* view);

		GpuBufferType getType() const { return mType; }
		GpuBufferUsage getUsage() const { return mUsage; }
		bool getRandomGpuWrite() const { return mRandomGpuWrite; }
		bool getUseCounter() const { return mUseCounter; }
		UINT32 getElementCount() const { return mElementCount; }
		UINT32 getElementSize() const { return mElementSize; }

	protected:
		GpuBufferType mType;
		GpuBufferUsage mUsage;
		bool mRandomGpuWrite;
		bool mUseCounter;
		UINT32 mElementCount;
		UINT32 mElementSize;

		virtual GpuBufferView* createView() = 0;
		virtual void destroyView(GpuBufferView* view) = 0;
		void clearBufferViews();

		/**
		 * @copydoc CoreGpuObject::destroy_internal()
		 */
		virtual void destroy_internal();

		struct GpuBufferReference
		{
			GpuBufferReference(GpuBufferView* _view)
				:view(_view), refCount(0)
			{ }

			GpuBufferView* view;
			UINT32 refCount;
		};

		std::unordered_map<GPU_BUFFER_DESC, GpuBufferReference*, GpuBufferView::HashFunction, GpuBufferView::EqualFunction> mBufferViews;
    };
}

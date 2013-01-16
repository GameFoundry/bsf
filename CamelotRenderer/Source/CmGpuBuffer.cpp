#include "CmGpuBuffer.h"
#include "CmException.h"

namespace CamelotEngine
{
	GpuBuffer::GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
		:mElementCount(elementCount), mElementSize(elementSize), mType(type), mUsage(usage), mRandomGpuWrite(randomGpuWrite), mUseCounter(useCounter)
	{  
	}

	GpuBuffer::~GpuBuffer() 
	{
		// Make sure that derived classes call clearBufferViews
		// I can't call it here since it needs a virtual method call
	}

	void GpuBuffer::clearBufferViews()
	{
		for(auto iter = mBufferViews.begin(); iter != mBufferViews.end(); ++iter)
		{
			destroyView(iter->second->view);
			delete iter->second;
		}

		mBufferViews.clear();
	}

	GpuBufferView* GpuBuffer::requestView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
	{
		GpuBufferView::Key key(firstElement, elementWidth, numElements, randomGpuWrite);

		auto iterFind = mBufferViews.find(key);
		if(iterFind == mBufferViews.end())
		{
			GpuBufferView* newView = createView(firstElement, elementWidth, numElements, randomGpuWrite);
			mBufferViews[key] = new GpuBufferReference(newView);

			iterFind = mBufferViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void GpuBuffer::releaseView(GpuBufferView* view)
	{
		GpuBufferView::Key key(*view);

		auto iterFind = mBufferViews.find(key);
		if(iterFind == mBufferViews.end())
		{
			CM_EXCEPT(InternalErrorException, "Trying to release a buffer that doesn't exist!");
		}

		iterFind->second->refCount--;

		if(iterFind->second->refCount == 0)
		{
			GpuBufferReference* toRemove = iterFind->second;

			mBufferViews.erase(iterFind);

			destroyView(toRemove->view);
			delete toRemove;
		}
	}
}
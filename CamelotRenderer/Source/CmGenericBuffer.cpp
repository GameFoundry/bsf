#include "CmGenericBuffer.h"
#include "CmException.h"

namespace CamelotEngine
{
	GenericBuffer::GenericBuffer(UINT32 elementCount, UINT32 elementSize, GenericBufferType type, GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
		:mElementCount(elementCount), mElementSize(elementSize), mType(type), mUsage(usage), mRandomGpuWrite(randomGpuWrite), mUseCounter(useCounter)
	{  
	}

	GenericBuffer::~GenericBuffer() 
	{
		// Make sure that derived classes call clearBufferViews
		// I can't call it here since it needs a virtual method call
	}

	void GenericBuffer::clearBufferViews()
	{
		for(auto iter = mBufferViews.begin(); iter != mBufferViews.end(); ++iter)
		{
			destroyView(iter->second->view);
			delete iter->second;
		}

		mBufferViews.clear();
	}

	GenericBufferView* GenericBuffer::requestView(UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool randomGpuWrite)
	{
		GenericBufferView::Key key(firstElement, elementWidth, numElements, randomGpuWrite);

		auto iterFind = mBufferViews.find(key);
		if(iterFind == mBufferViews.end())
		{
			GenericBufferView* newView = createView(firstElement, elementWidth, numElements, randomGpuWrite);
			mBufferViews[key] = new GenericBufferReference(newView);

			iterFind = mBufferViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void GenericBuffer::releaseView(GenericBufferView* view)
	{
		GenericBufferView::Key key(*view);

		auto iterFind = mBufferViews.find(key);
		if(iterFind == mBufferViews.end())
		{
			CM_EXCEPT(InternalErrorException, "Trying to release a buffer that doesn't exist!");
		}

		iterFind->second->refCount--;

		if(iterFind->second->refCount == 0)
		{
			GenericBufferReference* toRemove = iterFind->second;

			mBufferViews.erase(iterFind);

			destroyView(toRemove->view);
			delete toRemove;
		}
	}
}
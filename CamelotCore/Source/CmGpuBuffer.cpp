#include "CmGpuBuffer.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace CamelotFramework
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

	void GpuBuffer::destroy_internal()
	{
		clearBufferViews();

		CoreObject::destroy_internal();
	}

	void GpuBuffer::clearBufferViews()
	{
		for(auto iter = mBufferViews.begin(); iter != mBufferViews.end(); ++iter)
		{
			destroyView(iter->second->view);
			CM_DELETE(iter->second, GpuBufferReference, PoolAlloc);
		}

		mBufferViews.clear();
	}

	GpuBufferView* GpuBuffer::requestView(GpuBufferPtr buffer, UINT32 firstElement, UINT32 elementWidth, UINT32 numElements, bool useCounter, GpuViewUsage usage)
	{
		GPU_BUFFER_DESC key;
		key.firstElement = firstElement;
		key.elementWidth = elementWidth;
		key.numElements = numElements;
		key.numElements = numElements;
		key.usage = usage;

		auto iterFind = buffer->mBufferViews.find(key);
		if(iterFind == buffer->mBufferViews.end())
		{
			GpuBufferView* newView = buffer->createView();
			newView->initialize(buffer, key);
			buffer->mBufferViews[key] = cm_new<GpuBufferReference, PoolAlloc>(newView);

			iterFind = buffer->mBufferViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void GpuBuffer::releaseView(GpuBufferView* view)
	{
		GpuBufferPtr buffer = view->getBuffer();

		auto iterFind = buffer->mBufferViews.find(view->getDesc());
		if(iterFind == buffer->mBufferViews.end())
		{
			CM_EXCEPT(InternalErrorException, "Trying to release a buffer view that doesn't exist!");
		}

		iterFind->second->refCount--;

		if(iterFind->second->refCount == 0)
		{
			GpuBufferReference* toRemove = iterFind->second;

			buffer->mBufferViews.erase(iterFind);

			buffer->destroyView(toRemove->view);
			CM_DELETE(toRemove, GpuBufferReference, PoolAlloc);
		}
	}
}
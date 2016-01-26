//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsGpuBuffer.h"
#include "BsException.h"
#include "BsRenderAPI.h"
#include "BsHardwareBufferManager.h"

namespace BansheeEngine
{
	GpuBufferProperties::GpuBufferProperties(UINT32 elementCount, UINT32 elementSize, GpuBufferType type,
		GpuBufferUsage usage, bool randomGpuWrite, bool useCounter)
		:mElementCount(elementCount), mElementSize(elementSize), mType(type), mUsage(usage), mRandomGpuWrite(randomGpuWrite), mUseCounter(useCounter)
	{

	}

	GpuBufferCore::GpuBufferCore(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage,
		bool randomGpuWrite, bool useCounter)
		: mProperties(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{
	}

	GpuBufferCore::~GpuBufferCore()
	{
		// Make sure that derived classes call clearBufferViews
		// I can't call it here since it needs a virtual method call
	}

	void GpuBufferCore::clearBufferViews()
	{
		for (auto iter = mBufferViews.begin(); iter != mBufferViews.end(); ++iter)
		{
			destroyView(iter->second->view);
			bs_delete(iter->second);
		}

		mBufferViews.clear();
	}

	GpuBufferView* GpuBufferCore::requestView(const SPtr<GpuBufferCore>& buffer, UINT32 firstElement,
		UINT32 elementWidth, UINT32 numElements, bool useCounter, GpuViewUsage usage)
	{
		GPU_BUFFER_DESC key;
		key.firstElement = firstElement;
		key.elementWidth = elementWidth;
		key.numElements = numElements;
		key.numElements = numElements;
		key.usage = usage;

		auto iterFind = buffer->mBufferViews.find(key);
		if (iterFind == buffer->mBufferViews.end())
		{
			GpuBufferView* newView = buffer->createView();
			newView->initialize(buffer, key);
			buffer->mBufferViews[key] = bs_new<GpuBufferReference>(newView);

			iterFind = buffer->mBufferViews.find(key);
		}

		iterFind->second->refCount++;
		return iterFind->second->view;
	}

	void GpuBufferCore::releaseView(GpuBufferView* view)
	{
		SPtr<GpuBufferCore> buffer = view->getBuffer();

		auto iterFind = buffer->mBufferViews.find(view->getDesc());
		if (iterFind == buffer->mBufferViews.end())
		{
			BS_EXCEPT(InternalErrorException, "Trying to release a buffer view that doesn't exist!");
		}

		iterFind->second->refCount--;

		if (iterFind->second->refCount == 0)
		{
			GpuBufferReference* toRemove = iterFind->second;

			buffer->mBufferViews.erase(iterFind);

			buffer->destroyView(toRemove->view);
			bs_delete(toRemove);
		}
	}

	GpuBuffer::GpuBuffer(UINT32 elementCount, UINT32 elementSize, GpuBufferType type, GpuBufferUsage usage, 
		bool randomGpuWrite, bool useCounter)
		:mProperties(elementCount, elementSize, type, usage, randomGpuWrite, useCounter)
	{  
	}

	SPtr<GpuBufferCore> GpuBuffer::getCore() const
	{
		return std::static_pointer_cast<GpuBufferCore>(mCoreSpecific);
	}

	SPtr<CoreObjectCore> GpuBuffer::createCore() const
	{
		return HardwareBufferCoreManager::instance().createGpuBufferInternal(mProperties.getElementCount(), 
			mProperties.getElementSize(), mProperties.getType(), mProperties.getUsage(), mProperties.getRandomGpuWrite(), mProperties.getUseCounter());
	}
}
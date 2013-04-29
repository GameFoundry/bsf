#include "CmGpuResourceData.h"
#include "CmRenderSystem.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuResourceData::GpuResourceData()
		:mData(nullptr), mLocked(false), mOwnsData(false)
	{

	}

	GpuResourceData::GpuResourceData(const GpuResourceData& copy)
	{
		mData = copy.mData;
		mLocked = copy.mLocked; // TODO - This should be shared by all copies pointing to the same data?
		mOwnsData = false;
	}

	GpuResourceData::~GpuResourceData()
	{
		freeInternalBuffer();
	}

	UINT8* GpuResourceData::getData() const
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != RenderSystem::instance().getRenderThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-render thread when the buffer is locked.");
		}
#endif

		return mData;
	}

	void GpuResourceData::allocateInternalBuffer(UINT32 size)
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != RenderSystem::instance().getRenderThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-render thread when the buffer is locked.");
		}
#endif

		freeInternalBuffer();

		mData = CM_NEW_BYTES(size, ScratchAlloc);
		mOwnsData = true;
	}

	void GpuResourceData::freeInternalBuffer()
	{
		if(mData == nullptr || !mOwnsData)
			return;

#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != RenderSystem::instance().getRenderThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-render thread when the buffer is locked.");
		}
#endif

		CM_DELETE_BYTES(mData, ScratchAlloc);
		mData = nullptr;
	}

	void GpuResourceData::setExternalBuffer(UINT8* data)
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != RenderSystem::instance().getRenderThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-render thread when the buffer is locked.");
		}
#endif

		freeInternalBuffer();

		mData = data;
		mOwnsData = false;
	}

	void GpuResourceData::lock() const
	{
		mLocked = true;
	}

	void GpuResourceData::unlock() const
	{
		mLocked = false;
	}
}
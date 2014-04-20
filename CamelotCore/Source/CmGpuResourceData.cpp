#include "CmGpuResourceData.h"
#include "CmGpuResourceDataRTTI.h"
#include "CmCoreThread.h"
#include "CmException.h"

namespace BansheeEngine
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
			if(CM_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-core thread when the buffer is locked.");
		}
#endif

		return mData;
	}

	void GpuResourceData::allocateInternalBuffer()
	{
		allocateInternalBuffer(getInternalBufferSize());
	}

	void GpuResourceData::allocateInternalBuffer(UINT32 size)
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-core thread when the buffer is locked.");
		}
#endif

		freeInternalBuffer();

		mData = (UINT8*)cm_alloc<ScratchAlloc>(size);
		mOwnsData = true;
	}

	void GpuResourceData::freeInternalBuffer()
	{
		if(mData == nullptr || !mOwnsData)
			return;

#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-core thread when the buffer is locked.");
		}
#endif

		cm_free<ScratchAlloc>(mData);
		mData = nullptr;
	}

	void GpuResourceData::setExternalBuffer(UINT8* data)
	{
#if !CM_FORCE_SINGLETHREADED_RENDERING
		if(mLocked)
		{
			if(CM_THREAD_CURRENT_ID != CoreThread::instance().getCoreThreadId())
				CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data from non-core thread when the buffer is locked.");
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

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* GpuResourceData::getRTTIStatic()
	{
		return GpuResourceDataRTTI::instance();
	}

	RTTITypeBase* GpuResourceData::getRTTI() const
	{
		return GpuResourceData::getRTTIStatic();
	}
}
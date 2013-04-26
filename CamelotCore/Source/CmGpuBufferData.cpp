#include "CmGpuBufferData.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuBufferData::GpuBufferData()
		:mData(nullptr), mLocked(false)
	{

	}

	GpuBufferData::~GpuBufferData()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}

	UINT8* GpuBufferData::getData() const
	{
		if(mLocked)
			CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data when the buffer is locked.");

		return mData;
	}

	void GpuBufferData::initialize(UINT32 size)
	{
		mData = CM_NEW_BYTES(size, ScratchAlloc);
	}

	void GpuBufferData::lock()
	{
		mLocked = true;
	}

	void GpuBufferData::unlock()
	{
		mLocked = false;
	}

}
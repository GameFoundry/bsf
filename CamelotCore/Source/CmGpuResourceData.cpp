#include "CmGpuResourceData.h"
#include "CmException.h"

namespace CamelotFramework
{
	GpuResourceData::GpuResourceData()
		:mData(nullptr), mLocked(false)
	{

	}

	GpuResourceData::~GpuResourceData()
	{
		if(mData != nullptr)
			CM_DELETE_BYTES(mData, ScratchAlloc);
	}

	UINT8* GpuResourceData::getData() const
	{
		if(mLocked)
			CM_EXCEPT(InternalErrorException, "You are not allowed to access buffer data when the buffer is locked.");

		return mData;
	}

	void GpuResourceData::initialize(UINT32 size)
	{
		mData = CM_NEW_BYTES(size, ScratchAlloc);
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
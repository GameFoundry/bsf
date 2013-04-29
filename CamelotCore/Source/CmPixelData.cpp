#include "CmPixelData.h"
#include "CmPixelUtil.h"
#include "CmPixelDataRTTI.h"

namespace CamelotFramework
{
	PixelData::PixelData(const PixelData& copy)
		:IReflectable(copy)
	{
		data = copy.data;
		format = copy.format;
		rowPitch = copy.rowPitch;
		slicePitch = copy.slicePitch;
		mExtents = copy.mExtents;
		ownsData = false;
	}

	UINT8* PixelData::allocateInternalBuffer(UINT32 size)
	{
		data = CM_NEW_BYTES(size, ScratchAlloc);
		ownsData = true;

		return (UINT8*)data;
	}

	void PixelData::freeInternalBuffer()
	{
		if(ownsData && getData() != nullptr)
		{
			CM_DELETE_BYTES(getData(), ScratchAlloc);

			data = nullptr;
		}
	}

	void PixelData::setExternalBuffer(UINT8* data)
	{
		freeInternalBuffer();

		this->data = data;
		ownsData = false;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/

	RTTITypeBase* PixelData::getRTTIStatic()
	{
		return PixelDataRTTI::instance();
	}

	RTTITypeBase* PixelData::getRTTI() const
	{
		return PixelData::getRTTIStatic();
	}
}
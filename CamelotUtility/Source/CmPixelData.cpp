#include "CmPixelData.h"
#include "CmPixelUtil.h"
#include "CmPixelDataRTTI.h"

namespace CamelotFramework
{
	PixelData::PixelData(const PixelData& copy)
		:Box(copy), IReflectable(copy)
	{
		data = copy.data;
		format = copy.format;
		rowPitch = copy.rowPitch;
		slicePitch = copy.slicePitch;
		ownsData = false;
	}

	UINT8* PixelData::allocData(UINT32 size)
	{
		data = CM_NEW_BYTES(size, ScratchAlloc);
		ownsData = true;

		return (UINT8*)data;
	}

	void PixelData::freeData()
	{
		if(ownsData && getData() != nullptr)
		{
			CM_DELETE_BYTES(getData(), ScratchAlloc);

			data = nullptr;
		}
	}

	void PixelData::setExternalDataPtr(UINT8* data)
	{
		freeData();

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
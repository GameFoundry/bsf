#include "CmPixelData.h"
#include "CmPixelDataRTTI.h"

namespace CamelotEngine
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
#include "CmPixelData.h"
#include "CmPixelUtil.h"
#include "CmPixelDataRTTI.h"

namespace CamelotFramework
{
	PixelData::PixelData(const PixelData& copy)
		:GpuResourceData(copy)
	{
		mFormat = copy.mFormat;
		mRowPitch = copy.mRowPitch;
		mSlicePitch = copy.mSlicePitch;
		mExtents = copy.mExtents;
	}

	UINT32 PixelData::getInternalBufferSize()
	{
		return PixelUtil::getMemorySize(getWidth(), getHeight(), getDepth(), getFormat());
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
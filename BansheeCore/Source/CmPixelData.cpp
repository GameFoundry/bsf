#include "CmPixelData.h"
#include "CmPixelUtil.h"
#include "CmPixelDataRTTI.h"
#include "CmColor.h"

namespace BansheeEngine
{
	PixelData::PixelData(const PixelData& copy)
		:GpuResourceData(copy)
	{
		mFormat = copy.mFormat;
		mRowPitch = copy.mRowPitch;
		mSlicePitch = copy.mSlicePitch;
		mExtents = copy.mExtents;
	}

	PixelData& PixelData::operator=(const PixelData& rhs)
	{
		GpuResourceData::operator= (rhs);

		mFormat = rhs.mFormat;
		mRowPitch = rhs.mRowPitch;
		mSlicePitch = rhs.mSlicePitch;
		mExtents = rhs.mExtents;

		return *this;
	}

	UINT32 PixelData::getConsecutiveSize() const
	{
		return PixelUtil::getMemorySize(getWidth(), getHeight(), getDepth(), mFormat);
	}

	PixelData PixelData::getSubVolume(const PixelVolume &def) const
	{
		if (PixelUtil::isCompressed(mFormat))
		{
			if (def.left == getLeft() && def.top == getTop() && def.front == getFront() &&
				def.right == getRight() && def.bottom == getBottom() && def.back == getBack())
			{
				// Entire buffer is being queried
				return *this;
			}

			CM_EXCEPT(InvalidParametersException, "Cannot return subvolume of compressed PixelBuffer");
		}

		if (!mExtents.contains(def))
		{
			CM_EXCEPT(InvalidParametersException, "Bounds out of range");
		}

		const size_t elemSize = PixelUtil::getNumElemBytes(mFormat);
		PixelData rval(def.getWidth(), def.getHeight(), def.getDepth(), mFormat);

		rval.setExternalBuffer(((UINT8*)getData()) + ((def.left - getLeft())*elemSize)
			+ ((def.top - getTop())*mRowPitch*elemSize)
			+ ((def.front - getFront())*mSlicePitch*elemSize));

		rval.mRowPitch = mRowPitch;
		rval.mSlicePitch = mSlicePitch;
		rval.mFormat = mFormat;

		return rval;
	}

	Color PixelData::getColorAt(UINT32 x, UINT32 y, UINT32 z)
	{
		Color cv;

		UINT32 pixelSize = PixelUtil::getNumElemBytes(mFormat);
		UINT32 pixelOffset = pixelSize * (z * mSlicePitch + y * mRowPitch + x);
		PixelUtil::unpackColor(&cv, mFormat, (unsigned char *)getData() + pixelOffset);

		return cv;
	}

	void PixelData::setColorAt(Color const &cv, UINT32 x, UINT32 y, UINT32 z)
	{
		UINT32 pixelSize = PixelUtil::getNumElemBytes(mFormat);
		UINT32 pixelOffset = pixelSize * (z * mSlicePitch + y * mRowPitch + x);
		PixelUtil::packColor(cv, mFormat, (unsigned char *)getData() + pixelOffset);
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
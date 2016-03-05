//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsPixelData.h"
#include "BsRTTIType.h"
#include "BsManagedDataBlock.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Core
	 *  @{
	 */

	class BS_CORE_EXPORT PixelDataRTTI : public RTTIType<PixelData, GpuResourceData, PixelDataRTTI>
	{
		UINT32& getLeft(PixelData* obj) { return obj->mExtents.left; }
		void setLeft(PixelData* obj, UINT32& val) { obj->mExtents.left = val; }

		UINT32& getTop(PixelData* obj) { return obj->mExtents.top; }
		void setTop(PixelData* obj, UINT32& val) { obj->mExtents.top = val; }

		UINT32& getRight(PixelData* obj) { return obj->mExtents.right; }
		void setRight(PixelData* obj, UINT32& val) { obj->mExtents.right = val; }

		UINT32& getBottom(PixelData* obj) { return obj->mExtents.bottom; }
		void setBottom(PixelData* obj, UINT32& val) { obj->mExtents.bottom = val; }

		UINT32& getFront(PixelData* obj) { return obj->mExtents.front; }
		void setFront(PixelData* obj, UINT32& val) { obj->mExtents.front = val; }

		UINT32& getBack(PixelData* obj) { return obj->mExtents.back; }
		void setBack(PixelData* obj, UINT32& val) { obj->mExtents.back = val; }

		UINT32& getRowPitch(PixelData* obj) { return obj->mRowPitch; }
		void setRowPitch(PixelData* obj, UINT32& val) { obj->mRowPitch = val; }

		UINT32& getSlicePitch(PixelData* obj) { return obj->mSlicePitch; }
		void setSlicePitch(PixelData* obj, UINT32& val) { obj->mSlicePitch = val; }

		PixelFormat& getFormat(PixelData* obj) { return obj->mFormat; }
		void setFormat(PixelData* obj, PixelFormat& val) { obj->mFormat = val; }

		ManagedDataBlock getData(PixelData* obj) 
		{ 
			ManagedDataBlock dataBlock((UINT8*)obj->getData(), obj->getConsecutiveSize());
			return dataBlock; 
		}

		void setData(PixelData* obj, ManagedDataBlock val) 
		{ 
			// Nothing to do here, the pointer we provided already belongs to PixelData
			// so the data is already written
		}

		static UINT8* allocateData(PixelData* obj, UINT32 numBytes)
		{
			obj->allocateInternalBuffer(numBytes);

			return obj->getData();
		}

	public:
		PixelDataRTTI()
		{
			addPlainField("left", 0, &PixelDataRTTI::getLeft, &PixelDataRTTI::setLeft);
			addPlainField("top", 1, &PixelDataRTTI::getTop, &PixelDataRTTI::setTop);
			addPlainField("right", 2, &PixelDataRTTI::getRight, &PixelDataRTTI::setRight);
			addPlainField("bottom", 3, &PixelDataRTTI::getBottom, &PixelDataRTTI::setBottom);
			addPlainField("front", 4, &PixelDataRTTI::getFront, &PixelDataRTTI::setFront);
			addPlainField("back", 5, &PixelDataRTTI::getBack, &PixelDataRTTI::setBack);
			addPlainField("rowPitch", 6, &PixelDataRTTI::getRowPitch, &PixelDataRTTI::setRowPitch);
			addPlainField("slicePitch", 7, &PixelDataRTTI::getSlicePitch, &PixelDataRTTI::setSlicePitch);
			addPlainField("format", 8, &PixelDataRTTI::getFormat, &PixelDataRTTI::setFormat);

			addDataBlockField("data", 9, &PixelDataRTTI::getData, &PixelDataRTTI::setData, 0, &PixelDataRTTI::allocateData);
		}

		virtual const String& getRTTIName() override
		{
			static String name = "PixelData";
			return name;
		}

		virtual UINT32 getRTTIId() override
		{
			return TID_PixelData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject() override
		{
			PixelDataPtr newPixelData = bs_shared_ptr_new<PixelData>();

			return newPixelData;
		}
	};

	/** @} */
	/** @endcond */
}
#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmPixelData.h"
#include "CmRTTIType.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT PixelDataRTTI : public RTTIType<PixelData, IReflectable, PixelDataRTTI>
	{
		UINT32& getLeft(PixelData* obj) { return obj->left; }
		void setLeft(PixelData* obj, UINT32& val) { obj->left = val; }

		UINT32& getTop(PixelData* obj) { return obj->top; }
		void setTop(PixelData* obj, UINT32& val) { obj->top = val; }

		UINT32& getRight(PixelData* obj) { return obj->right; }
		void setRight(PixelData* obj, UINT32& val) { obj->right = val; }

		UINT32& getBottom(PixelData* obj) { return obj->bottom; }
		void setBottom(PixelData* obj, UINT32& val) { obj->bottom = val; }

		UINT32& getFront(PixelData* obj) { return obj->front; }
		void setFront(PixelData* obj, UINT32& val) { obj->front = val; }

		UINT32& getBack(PixelData* obj) { return obj->back; }
		void setBack(PixelData* obj, UINT32& val) { obj->back = val; }

		UINT32& getRowPitch(PixelData* obj) { return obj->rowPitch; }
		void setRowPitch(PixelData* obj, UINT32& val) { obj->rowPitch = val; }

		UINT32& getSlicePitch(PixelData* obj) { return obj->slicePitch; }
		void setSlicePitch(PixelData* obj, UINT32& val) { obj->slicePitch = val; }

		ManagedDataBlock getData(PixelData* obj) 
		{ 
			ManagedDataBlock dataBlock((UINT8*)obj->data, obj->getConsecutiveSize(), false);
			return dataBlock; 
		}

		void setData(PixelData* obj, ManagedDataBlock val) 
		{ 
			obj->data = val.getData();
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

			addDataBlockField("data", 8, &PixelDataRTTI::getData, &PixelDataRTTI::setData);
		}

		virtual const String& getRTTIName()
		{
			static String name = "PixelData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_PixelData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			PixelDataPtr newPixelData(new PixelData());
			newPixelData->ownsData = true;

			return newPixelData;
		}
	};
}
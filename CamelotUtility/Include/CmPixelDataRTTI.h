#pragma once

#include "CmPrerequisitesUtil.h"
#include "CmPixelData.h"
#include "CmRTTIType.h"
#include "CmManagedDataBlock.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT PixelDataRTTI : public RTTIType<PixelData, IReflectable, PixelDataRTTI>
	{
		size_t& getLeft(PixelData* obj) { return obj->left; }
		void setLeft(PixelData* obj, size_t& val) { obj->left = val; }

		size_t& getTop(PixelData* obj) { return obj->top; }
		void setTop(PixelData* obj, size_t& val) { obj->top = val; }

		size_t& getRight(PixelData* obj) { return obj->right; }
		void setRight(PixelData* obj, size_t& val) { obj->right = val; }

		size_t& getBottom(PixelData* obj) { return obj->bottom; }
		void setBottom(PixelData* obj, size_t& val) { obj->bottom = val; }

		size_t& getFront(PixelData* obj) { return obj->front; }
		void setFront(PixelData* obj, size_t& val) { obj->front = val; }

		size_t& getBack(PixelData* obj) { return obj->back; }
		void setBack(PixelData* obj, size_t& val) { obj->back = val; }

		size_t& getRowPitch(PixelData* obj) { return obj->rowPitch; }
		void setRowPitch(PixelData* obj, size_t& val) { obj->rowPitch = val; }

		size_t& getSlicePitch(PixelData* obj) { return obj->slicePitch; }
		void setSlicePitch(PixelData* obj, size_t& val) { obj->slicePitch = val; }

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
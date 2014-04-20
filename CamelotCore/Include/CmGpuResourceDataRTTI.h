#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGpuResourceData.h"

namespace BansheeEngine
{
	class CM_EXPORT GpuResourceDataRTTI : public RTTIType<GpuResourceData, IReflectable, GpuResourceDataRTTI>
	{
	public:
		GpuResourceDataRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "GpuResourceData";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GpuResourceData;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
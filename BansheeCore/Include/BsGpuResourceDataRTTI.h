#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuResourceData.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GpuResourceDataRTTI : public RTTIType<GpuResourceData, IReflectable, GpuResourceDataRTTI>
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
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
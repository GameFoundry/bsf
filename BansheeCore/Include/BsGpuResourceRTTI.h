#pragma once

#include "BsCorePrerequisites.h"
#include "BsRTTIType.h"
#include "BsGpuResource.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT GpuResourceRTTI : public RTTIType<GpuResource, Resource, GpuResourceRTTI>
	{
	public:
		GpuResourceRTTI()
		{

		}

		virtual const String& getRTTIName()
		{
			static String name = "GpuResource";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GpuResource;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			BS_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
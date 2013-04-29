#pragma once

#include "CmPrerequisites.h"
#include "CmRTTIType.h"
#include "CmGpuResource.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuResourceRTTI : public RTTIType<GpuResource, Resource, GpuResourceRTTI>
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
			CM_EXCEPT(InternalErrorException, "Cannot instantiate an abstract class.");
		}
	};
}
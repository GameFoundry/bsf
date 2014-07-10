//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
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
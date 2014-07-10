//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGpuResource.h"
#include "BsGpuResourceRTTI.h"

namespace BansheeEngine
{
	RTTITypeBase* GpuResource::getRTTIStatic()
	{
		return GpuResourceRTTI::instance();
	}

	RTTITypeBase* GpuResource::getRTTI() const
	{
		return GpuResource::getRTTIStatic();
	}
}